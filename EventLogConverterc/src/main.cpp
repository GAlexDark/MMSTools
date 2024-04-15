/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: main.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Main file
*
****************************************************************************/

#include <QCoreApplication>
#include <QDir>
#include <QMap>

#include "MMSTypes.h"
//#include "Debug.h"
#include "CSVReader.h"
#include "CReportBuilder.h"
#include "CElcConsoleAppSettings.h"
#include "CSingleApplication.h"
#include "QCommandLineParserHelper.h"
#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "CParserManager.h"
#include "CReportManager.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));
    QString appName = QCoreApplication::applicationName();
    QString appPath = a.applicationDirPath();

    CConsoleOutput consoleOut;
    QString description = QStringLiteral("MMS Event Log Conversion Utility Version %1\nCopyright (C) 2023 Oleksii Gaienko, %3\n\n");
    description.append(QStringLiteral("This program uses Qt version %2 and QXlsx library: https://github.com/QtExcel/QXlsx.\n"));
    consoleOut.outToConsole(description.arg(a.applicationVersion(), QT_VER, CONTACT));

    CSingleApplication sa(QLatin1String("elcc_instance"));
    if (sa.isRunning()) {
        consoleOut.outToConsole(QLatin1String("The another copy of the Utility is still running."));
        return 1;
    }

    QCommandLineParserHelper cmd;
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
    QString dbName =  QDir::fromNativeSeparators(settings.getMain(QLatin1String("SETTINGS/db_file_name")).toString().trimmed());
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1/%2.db").arg(appPath, appName);
        settings.setMain(QLatin1String("SETTINGS"), QLatin1String("db_file_name"), dbName);
        consoleOut.outToConsole(QLatin1String("Unable to get database file name.\nThe database file will be created on the default path."));
    }
    QString errorString;
    elcUtils::expandEnvironmentStrings(dbName);
    QString dbPath = QFileInfo(dbName).absolutePath();
    if (!elcUtils::mkPath(dbPath, errorString)) {
        consoleOut.outToConsole(QStringLiteral("Cannot create folder: %1\nDetails: %2").arg(dbPath, errorString));
        return 1;
    }

/* PREPARE */

    RunningMode runningMode = cmd.getRunningMode();
    if (runningMode == RUNNINGMODE_DEFAULT || runningMode == RUNNINGMODE_CLEAN_DB) {
        QString cleardb = settings.getMain(QLatin1String("SETTINGS/clear_on_startup")).toString().trimmed();
        bool comb1 = cleardb.isEmpty() || (QString::compare(cleardb, QLatin1String("yes"), Qt::CaseInsensitive) == 0);
        if (runningMode == RUNNINGMODE_CLEAN_DB) { //ignoring settings
            comb1 = true;
        }
        if (comb1) {
            consoleOut.outToConsole(QLatin1String("Starting cleaning database..."));
            const CParserManager &parserManager = CParserManager::instance();
            qsizetype count = parserManager.getItemCount();
            QStringList tables = parserManager.getTablesList();
            QStringList creationStrings = parserManager.getCreateTableRequestList();
            if (!elcUtils::trunvateDB(dbName, errorString, count, tables, creationStrings)) {
                consoleOut.outToConsole(QLatin1String("Cannot clean database: %1").arg(errorString));
                return 1;
            }
            consoleOut.outToConsole(QLatin1String("The database was clean."));
        }
    }

/* IMPORT */

    if (runningMode != RUNNINGMODE_REPORT_ONLY && runningMode != RUNNINGMODE_CLEAN_DB) {
        QString internalIpFirstOctet = settings.getMain(QLatin1String("SETTINGS/internal_ip_start_octet")).toString().trimmed();
        if (internalIpFirstOctet.isEmpty() || !elcUtils::sanitizeValue(QLatin1String("^([0-9.]+)$"), internalIpFirstOctet)) {
            consoleOut.outToConsole(QLatin1String("Error in internal IP address mask. Please check it in the config file."));
            return 1;
        }

        QString value = settings.getMain(QLatin1String("SETTINGS/data_has_headers")).toString().trimmed();
        bool hasHeaders = (value.isEmpty() || QString::compare(value, QLatin1String("yes"), Qt::CaseInsensitive) == 0)? true : false;

        mms::pragmaList_t pragmaList;
        value = settings.getMain(QLatin1String("DATABASE/synchronous")).toString().trimmed();
        pragmaList["synchronous"] = elcUtils::sanitizeValue(value, elcUtils::plSynchronous, elcUtils::pvNormal);

        value = settings.getMain(QLatin1String("DATABASE/journal_mode")).toString().trimmed();
        pragmaList["journal_mode"] = elcUtils::sanitizeValue(value, elcUtils::plJournalMode, elcUtils::pvMemory);

        value = settings.getMain(QLatin1String("DATABASE/temp_store")).toString().trimmed();
        pragmaList["temp_store"] = elcUtils::sanitizeValue(value, elcUtils::plTempStore, elcUtils::pvMemory);

        value = settings.getMain(QLatin1String("DATABASE/locking_mode")).toString().trimmed();
        pragmaList["locking_mode"] = elcUtils::sanitizeValue(value, elcUtils::plLockingMode, elcUtils::pvExclusive);

        QStringList files;
        if (!cmd.getDataFilesList(files)) {
            consoleOut.outToConsole(cmd.errorString());
            return 1;
        }

        consoleOut.outToConsole(QLatin1String("Start reading and converting files..."));
        
        CMmsLogsThreadReader loader;
        bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
        Q_ASSERT_X(retVal, "connect", "connection is not established");

        quint16 logID = 1;
        retVal = loader.init(logID, dbName, hasHeaders, internalIpFirstOctet, &pragmaList);
        if (retVal) {
            loader.setFileName(files);
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

    if (runningMode != RUNNINGMODE_IMPORT_ONLY && runningMode != RUNNINGMODE_CLEAN_DB) {
        QString reportName = cmd.getReportName();

        QStringList excludedUsers, includedUsers;
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

        CSVThreadReportBuilder report;
        quint16 logID = 1;
        bool retVal = report.init(logID, dbName, reportName, &excludedUsers, &includedUsers);
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
            consoleOut.outToConsole(QStringLiteral("Report generating finished.\nThe result in the %1 file.").arg(reportName));
        } else {
            consoleOut.outToConsole(QStringLiteral("Error generate report: %1").arg(report.errorString()));
            return 1;
        }
    }

    return 0;
}
