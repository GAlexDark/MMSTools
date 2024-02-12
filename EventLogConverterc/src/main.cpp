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

//#include "Debug.h"
#include "CSVLoader.h"
#include "CReportBuilder.h"
#include "CElcConsoleAppSettings.h"
#include "CSingleApplication.h"
#include "QCommandLineParserHelper.h"
#include "CConsoleOutput.h"
#include "elcUtils.h"

using pragmaList_t = QMap<QString, QString>;

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

    CSingleApplication sa(QStringLiteral("elcc_instance"));
    if (sa.isRunning()) {
        consoleOut.outToConsole(QStringLiteral("The another copy of the Utility is still running."));
        return 1;
    }

    QCommandLineParserHelper cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QStringLiteral("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    }

    consoleOut.outToConsole(QStringLiteral("MMS Event Log Conversion Utility starting..."));

/* INIT */

    QString iniFile = QStringLiteral("%1.ini").arg(appName);
    if (!CElcConsoleAppSettings::instance().init(appPath, iniFile, false)) {
        consoleOut.outToConsole(QStringLiteral("The settings class cannot be initialized."));
        return 1;
    }
    CElcConsoleAppSettings &settings = CElcConsoleAppSettings::instance();
    QString dbName =  QDir::fromNativeSeparators(settings.getMain(SettingsDbFileName).toString().trimmed());
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1/%2.db").arg(appPath, appName);
        settings.setMain(SettingsGroup, KeyDbFileName, dbName);
        consoleOut.outToConsole(QStringLiteral("Unable to get database file name.\nThe database file will be created on the default path."));
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
        QString cleardb = settings.getMain(SettingsClearOnStartup).toString().trimmed();
        if (cleardb.isEmpty() || (QString::compare(cleardb, QLatin1String("yes"), Qt::CaseInsensitive) == 0)) {
            consoleOut.outToConsole(QStringLiteral("Starting cleaning database..."));
            if (!elcUtils::trunvateDB(dbName, errorString)) {
                consoleOut.outToConsole(QStringLiteral("Cannot clean database: %1").arg(errorString));
                return 1;
            }
            consoleOut.outToConsole(QStringLiteral("The database was clean."));
        }
    }

/* IMPORT */

    if (runningMode != RUNNINGMODE_REPORT_ONLY && runningMode != RUNNINGMODE_CLEAN_DB) {
        QString internalIpFirstOctet = settings.getMain(SettingsInternalIpStartOctet).toString().trimmed();
        if (internalIpFirstOctet.isEmpty() || !elcUtils::sanitizeValue(QStringLiteral("^([0-9.]+)$"), internalIpFirstOctet)) {
            consoleOut.outToConsole(QStringLiteral("Error in internal IP address mask. Please check it in the config file."));
            return 1;
        }

        QString value = settings.getMain(SettingsDataHasHeaders).toString().trimmed();
        bool hasHeaders = (value.isEmpty() || QString::compare(value, QLatin1String("yes"), Qt::CaseInsensitive) == 0)? true : false;

        pragmaList_t pragmaList;
        value = settings.getMain(DatabaseSynchronous).toString().trimmed();
        pragmaList["synchronous"] = elcUtils::sanitizeValue(value, elcUtils::plSynchronous, elcUtils::pvNormal);

        value = settings.getMain(DatabaseJournalMode).toString().trimmed();
        pragmaList["journal_mode"] = elcUtils::sanitizeValue(value, elcUtils::plJournalMode, elcUtils::pvMemory);

        value = settings.getMain(DatabaseTempStore).toString().trimmed();
        pragmaList["temp_store"] = elcUtils::sanitizeValue(value, elcUtils::plTempStore, elcUtils::pvMemory);

        value = settings.getMain(DatabaseLockingMode).toString().trimmed();
        pragmaList["locking_mode"] = elcUtils::sanitizeValue(value, elcUtils::plLockingMode, elcUtils::pvExclusive);

        QStringList files;
        if (!cmd.getDataFilesList(files)) {
            consoleOut.outToConsole(cmd.errorString());
            return 1;
        }

        consoleOut.outToConsole("Start reading and converting files...");

        CEventLogThreadLoader loader;
        bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
        Q_ASSERT_X(retVal, "connect", "connection is not established");

        retVal = loader.init(dbName, hasHeaders, internalIpFirstOctet, &pragmaList, "\r\n");
        if (retVal) {
            loader.setFileName(files);
            loader.start();

            consoleOut.outToConsole(QStringLiteral("wait..."));
            elcUtils::waitForEndThread(&loader, 100);
            retVal = loader.getStatus();
        }
        QCoreApplication::processEvents();
        QObject::disconnect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
        pragmaList.clear();

        if (retVal) {
            consoleOut.outToConsole(QStringLiteral("Reading file(s) completed"));
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

        consoleOut.outToConsole(QStringLiteral("Start generating the report..."));

        CSVThreadReportBuilder report;
        bool retVal = report.init(dbName, reportName, &excludedUsers, &includedUsers);
        if (retVal) {
            report.start();

            consoleOut.outToConsole(QStringLiteral("wait..."));
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
