/****************************************************************************
*
* The Event Log Conversion console Utility Main file.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#include <QCoreApplication>
#include <QFileInfo>

#include "MMSTypes.h"
#include "CSVReader.h"
#include "CReportBuilder.h"
#include "CElcConsoleAppSettings.h"
#include "CSingleApplication.h"
#include "CElcCmdLineParser.h"
#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "CParserManager.h"
#include "CReportManager.h"
#include "CSqliteDatabase.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationDirPath();

    CConsoleOutput consoleOut;
    QString description(QStringLiteral("MMS Event Log Conversion Utility Version %1\nCopyright (C) 2023 Oleksii Gaienko, %3\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.\n\n"));
    description.append(QStringLiteral("This program use Qt version %2 and QXlsx library: https://github.com/QtExcel/QXlsx.\n"));
    consoleOut.outToConsole(description.arg(QCoreApplication::applicationVersion(), QT_VER, CONTACT));

    CSingleApplication sa(QLatin1String("elcc_instance"));
    if (sa.isRunning()) {
        consoleOut.outToConsole(QLatin1String("The another copy of the Utility is still running."));
        return 1;
    }

    CElcCmdLineParser cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QLatin1String("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    }

    consoleOut.outToConsole(QLatin1String("MMS Event Log Conversion Utility starting..."));

/* INIT */

#ifdef Q_OS_WIN
    QString iniFile = QLatin1String("%1.ini").arg(appName);
#else
    QString fileName = QLatin1String("%1.conf").arg(appName);
#endif
    if (!CElcConsoleAppSettings::instance().init(appPath, iniFile, false)) {
        consoleOut.outToConsole(QLatin1String("The settings class cannot be initialized."));
        return 1;
    }
    CParserManager::instance().init();
    CElcConsoleAppSettings &settings = CElcConsoleAppSettings::instance();
    QString dbName =  settings.getDbFileName();
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1/%2.db").arg(appPath, appName);
        settings.setMain(QLatin1String("SETTINGS"), QLatin1String("db_file_name"), dbName);
        consoleOut.outToConsole(QLatin1String("Unable to get database file name.\nThe database file will be created on the default path."));
    }
    elcUtils::expandEnvironmentStrings(dbName);
    QString dbPath = QFileInfo(dbName).absolutePath();
    QString errorString;
    if (!elcUtils::mkPath(dbPath, errorString)) {
        consoleOut.outToConsole(QStringLiteral("Cannot create folder: %1\nDetails: %2").arg(dbPath, errorString));
        return 1;
    }

/* PREPARE */

    RunningMode runningMode = cmd.getRunningMode();
    if (runningMode == RunningMode::RUNNINGMODE_DEFAULT || runningMode == RunningMode::RUNNINGMODE_CLEAN_DB) {
        bool comb1 = settings.isClearDbOnStartup();
        if (runningMode == RunningMode::RUNNINGMODE_CLEAN_DB) { //ignoring settings
            comb1 = true;
        }
        if (comb1) {
            consoleOut.outToConsole(QLatin1String("Starting cleaning database..."));
            const CParserManager &parserManager = CParserManager::instance();
            qsizetype count = parserManager.getItemCount();
            QStringList tables = parserManager.getTablesList();
            if (!CSqliteDatabase::truncateDB(dbName, errorString, count, tables)) {
                consoleOut.outToConsole(QLatin1String("Cannot clean database: %1").arg(errorString));
                return 1;
            }
            consoleOut.outToConsole(QLatin1String("The database was clean."));
        }
    }

/* IMPORT */

    if (runningMode != RunningMode::RUNNINGMODE_REPORT_ONLY && runningMode != RunningMode::RUNNINGMODE_CLEAN_DB) {
        QString internalIpFirstOctet = settings.getInternalIpStartOctet();
        if (internalIpFirstOctet.isEmpty()) {
            consoleOut.outToConsole(QLatin1String("Error in internal IP address mask. Please check it in the config file."));
            return 1;
        }

        QStringList files;
        if (!cmd.getDataFilesList(files)) {
            consoleOut.outToConsole(cmd.errorString());
            return 1;
        }

        mms::pragmaList_t pragmaList;
        pragmaList["synchronous"] = settings.getSynchronousType();
        pragmaList["journal_mode"] = settings.getJournalModeType();
        pragmaList["temp_store"] = settings.getTempStore();
        pragmaList["locking_mode"] = settings.getLockingMode();

        consoleOut.outToConsole(QLatin1String("Start reading and converting files..."));

        CMmsLogsThreadReader loader;
        bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
        Q_ASSERT_X(retVal, "connect", "connection is not established");

        quint16 logID = 1;
        bool hasHeaders = settings.isDataHasHeaders();
        retVal = loader.init(logID, dbName, hasHeaders, internalIpFirstOctet, &pragmaList);
        if (retVal) {
            loader.setFileNames(files);
            loader.start();

            consoleOut.outToConsole(QLatin1String("wait..."));
            elcUtils::waitForEndThread(&loader, 100);
            retVal = loader.getStatus();
        }
        QCoreApplication::processEvents();
        QObject::disconnect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
        pragmaList.clear();

        if (retVal) {
            consoleOut.outToConsole(QLatin1String("Reading file(s) completed"));
        } else {
            consoleOut.outToConsole(QStringLiteral("Error reading file(s): %1").arg(loader.errorString()));
            return 1;
        }
    }
/* REPORTING */

    if (runningMode != RunningMode::RUNNINGMODE_IMPORT_ONLY && runningMode != RunningMode::RUNNINGMODE_CLEAN_DB) {
        QString reportName = cmd.getReportName();
        QString path = QFileInfo(reportName).absolutePath();
        bool retVal = elcUtils::isFolderWritable(path);
        if (retVal) {
            QStringList excludedUsers;
            QStringList includedUsers;
            if (!cmd.getExcludedUserNames(excludedUsers)) {
                consoleOut.outToConsole(cmd.errorString());
                return 1;
            }

            if (!cmd.getIncludedUserNames(includedUsers)) {
                consoleOut.outToConsole(cmd.errorString());
                return 1;
            }

            CReportManager::instance().init();
            consoleOut.outToConsole(QLatin1String("Start generating the report..."));

            bool showMilliseconds = settings.getShowMilliseconds();
            CSVThreadReportBuilder report;
            quint16 logID = 1;
            retVal = report.init(logID, dbName, reportName, &excludedUsers, &includedUsers, showMilliseconds);
            if (retVal) {
                report.start();

                consoleOut.outToConsole(QLatin1String("wait..."));
                elcUtils::waitForEndThread(&report, 100);
                retVal = report.getStatus();
            }
            QCoreApplication::processEvents();

            excludedUsers.clear();
            includedUsers.clear();
            if (retVal) {
                consoleOut.outToConsole(QStringLiteral("Report generating finished.\nThe report was saved in the %1 file.").arg(reportName));
            } else {
                consoleOut.outToConsole(QStringLiteral("Error generate report: %1").arg(report.errorString()));
                return 1;
            }
        } else {
            consoleOut.outToConsole(QStringLiteral("You don't have write permissions to this folder: %1").arg(path));
            return 1;
        }
    }
    return 0;
}
