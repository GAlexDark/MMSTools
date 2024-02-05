#include <QCoreApplication>
#include <QDir>
#include <QMap>

//#include "Debug.h"
#include "CSVLoader.h"
#include "CReportBuilder.h"
#include "CElcConsoleAppSettings.h"
#include "CSingleApplication.h"
#include "CDataSourceList.h"
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

    CSingleApplication sa("elc_instance");
    if (sa.isRunning()) {
        consoleOut.outToConsole(QStringLiteral("The another copy of the Utility is still running."));
        return 1;
    }

    QString searchFolder;
    QString reportName;
    QStringList files;
    QStringList excludedUsers, includedUsers;

    QCommandLineParserHelper cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(QStringLiteral("The fatal error has occurredd. The program will be closed."));
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QStringLiteral("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    } else {
        searchFolder = cmd.path();

        files = cmd.files();

        if (searchFolder.isEmpty() && files.isEmpty()) {
            consoleOut.outToConsole(QStringLiteral("The <path> and <files> arguments are missing."));
            return 1;
        }

        if (!files.isEmpty()) {
            QFileInfo fi;
            for (qsizetype i = 0; i < files.size(); ++i) {
                fi.setFile(files.at(i));
                if (fi.exists() && fi.isFile()) {
                    files[i] = fi.absoluteFilePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                } else {
                    consoleOut.outToConsole(QStringLiteral("The file %1 is corrupted or missing.").arg(fi.fileName()));
                    return 1;
                }
            }
        }

        if (!searchFolder.isEmpty()) {
            QFileInfo sf(searchFolder);
            QDir dir = sf.absoluteDir();
            QString mask = sf.fileName().trimmed();
            if (mask.isEmpty()) {
                mask = "*.csv"; // default mask
            }
            if (dir.exists()) {
                searchFolder = dir.absolutePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                files.append( CDataSourceList::getDataSourceList(searchFolder, QStringList() << mask) );
            } else {
                consoleOut.outToConsole(QStringLiteral("Cannot find the directory %1.").arg(searchFolder));
                return 1;
            }
        }
        files.removeDuplicates();

        reportName = cmd.reportName();
        if (reportName.isEmpty()) {
            QDateTime now = QDateTime::currentDateTime();
            reportName = QString("%1_report.xlsx").arg(now.toString("ddMMyyyy-hhmmsszzz"));
        }
        if ((reportName.indexOf('/') == -1) || (reportName.indexOf('\\') == -1)) {
            QString filePath = QFileInfo(files.at(0)).absolutePath();
            reportName = QDir(filePath).filePath(reportName);
        }
        if (!reportName.endsWith(".xlsx", Qt::CaseInsensitive)) {
            reportName = reportName + ".xlsx";
        }
        reportName = QDir::fromNativeSeparators(reportName);
        reportName.replace("//", "/");

        excludedUsers = cmd.excludedUsernames();
        QString buf;
        for (qsizetype i = 0; i < excludedUsers.size(); ++i) {
            buf = excludedUsers.at(i);
            if (!elcUtils::sanitizeValue(buf)) {
                consoleOut.outToConsole(QStringLiteral("Invalid character in the value %1").arg(buf));
                return 1;
            }
        }
        buf.clear();
        includedUsers = cmd.includedUsernames();
        for (qsizetype i = 0; i < includedUsers.size(); ++i) {
            buf = includedUsers.at(i);
            if (!elcUtils::sanitizeValue(buf)) {
                consoleOut.outToConsole(QStringLiteral("Invalid character in the value %1").arg(buf));
                return 1;
            }
        }
    }

    consoleOut.outToConsole(QStringLiteral("MMS Event Log Conversion Utility starting..."));
    QString errorString;
    QString iniFile = QStringLiteral("%1.ini").arg(appName);
    if (!CElcConsoleAppSettings::instance().init(appPath, iniFile, false)) {
        consoleOut.outToConsole(QStringLiteral("The settings class cannot be initialized."));
        return 1;
    }

    //get path to the DB
    CElcConsoleAppSettings &settings = CElcConsoleAppSettings::instance();
    QString dbName =  QDir::fromNativeSeparators(settings.getMain("SETTINGS/db_file_name").toString().trimmed());
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1/%2.db").arg(appPath, appName);
        settings.setMain(QStringLiteral("SETTINGS"), QStringLiteral("db_file_name"), dbName);
        consoleOut.outToConsole(QStringLiteral("Unable to get database file name.\n \
The database file will be created on the default path."));
    }
    elcUtils::expandEnvironmentStrings(dbName);
    QString dbPath = QFileInfo(dbName).absolutePath();
    if (!elcUtils::mkPath(dbPath, errorString)) {
        consoleOut.outToConsole(QStringLiteral("Cannot create folder: %1\nDetails: %2").arg(dbPath, errorString));
        return 1;
    }
    QString cleardb = settings.getMain("SETTINGS/clear_on_startup").toString().trimmed();
    if (cleardb.isEmpty() || (QString::compare(cleardb, "yes", Qt::CaseInsensitive) == 0)) {
        consoleOut.outToConsole(QStringLiteral("Starting cleaning database..."));
        if (!elcUtils::trunvateDB(dbName, errorString)) {
            consoleOut.outToConsole(QStringLiteral("Cannot clean database: %1").arg(errorString));
            return 1;
        }
        consoleOut.outToConsole(QStringLiteral("The database was clean."));
    }

    consoleOut.outToConsole("Start reading and converting files...");

    CEventLogThreadLoader loader;
    bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
    Q_ASSERT_X(retVal, "connect", "connection is not established");

    QString value = settings.getMain("SETTINGS/data_has_headers").toString().trimmed();
    bool hasHeaders = (value.isEmpty() || QString::compare(value, "yes", Qt::CaseInsensitive) == 0)? true : false;
    QString internalIpFirstOctet = settings.getMain("SETTINGS/internal_ip_start_octet").toString().trimmed();
    if (!internalIpFirstOctet.isEmpty() && elcUtils::sanitizeValue("^([0-9.]+)$", internalIpFirstOctet)) {
        pragmaList_t pragmaList;
        value = settings.getMain("DATABASE/synchronous").toString().trimmed();
        pragmaList["synchronous"] = elcUtils::sanitizeValue(value, QStringList() << "OFF" << "NORMAL" << "FULL", "NORMAL");

        value = settings.getMain("DATABASE/journal_mode").toString().trimmed();
        pragmaList["journal_mode"] = elcUtils::sanitizeValue(value,
                                                             QStringList() << "DELETE" << "TRUNCATE" << "PERSIST" << "MEMORY" << "WAL" << "OFF",
                                                             "MEMORY");

        value = settings.getMain("DATABASE/temp_store").toString().trimmed();
        pragmaList["temp_store"] = elcUtils::sanitizeValue(value,
                                                           QStringList() << "DEFAULT" << "FILE" << "MEMORY",
                                                           "MEMORY");

        value = settings.getMain("DATABASE/locking_mode").toString().trimmed();
        pragmaList["locking_mode"] = elcUtils::sanitizeValue(value,
                                                             QStringList() << "NORMAL" << "EXCLUSIVE",
                                                             "EXCLUSIVE");

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
    } else {
        consoleOut.outToConsole(QStringLiteral("Error in internal IP address mask. Please check it in the config file."));
        return 1;
    }

    consoleOut.outToConsole(QStringLiteral("Start generating the report..."));
    CSVThreadReportBuilder report;
    if (report.init(dbName, reportName, &excludedUsers, &includedUsers)) {
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

    return 0;
}
