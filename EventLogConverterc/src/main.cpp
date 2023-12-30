#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>
#include "Debug.h"
#include "CSVLoader.h"
#include "CReportBuilder.h"
#include "ELCSettings.h"
#include "CBasicDBClass.h"
#include "CSingleApplication.h"
#include "CDataSourceList.h"
#include "QCommandLineParserHelper.h"
#include "CConsoleOutput.h"

bool
trunvateDB(const QString &connectionString, QString &errorString)
{
    __DEBUG( Q_FUNC_INFO )

    CBasicDBClass db;
    bool retVal = db.init("QSQLITE", connectionString);
    if (retVal) {
        retVal = db.open();
        if (retVal) {
            retVal = db.truncateTable("eventlog");
            db.close();
        }
    }
    if (!retVal) {
        errorString = db.errorString();
    }
    return retVal;
}

bool
sanitizeValue(const QString &value)
{
    bool retVal = false;
    QRegularExpression mask("^([a-zA-Z0-9_]+)$");
    QRegularExpressionMatch match = mask.match(value);
    if (match.hasMatch()) {
        retVal = true;
    }
    return retVal;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, BUILD_DATE));
    QString appName = QCoreApplication::applicationName();
    QString appPath = a.applicationDirPath();
    QCoreApplication::addLibraryPath(QStringLiteral("%1/plugins").arg(appPath));

    CConsoleOutput consoleOut;
    QString description = QStringLiteral("MMS Event Log Conversion Utility Version %1\nCopyright (C) 2023 Oleksii Gaienko\n");
    description.append(QStringLiteral("This program uses Qt version %2 and QXlsx library: https://github.com/QtExcel/QXlsx.\n"));
    consoleOut.outToConsole(description.arg(a.applicationVersion(), QT_VER));

    CSingleApplication sa("elc_instance");
    if (sa.isRunning()) {
        consoleOut.outToConsole(QStringLiteral("The another copy of the Utility still running."));
        return 1;
    }

    QString searchFolder;
    QString reportName;
    QStringList files;
    QStringList excludedUsers;

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
        __DEBUG ( searchFolder )

        files = cmd.files();
        __DEBUG ( files )

        if (searchFolder.isEmpty() && files.isEmpty()) {
            consoleOut.outToConsole(QStringLiteral("The <path> and <files> arguments are missing."));
            return 0;
        }

        if (!files.isEmpty()) {
            QString buf;
            for (qsizetype i = 0; i < files.size(); ++i) {
                buf = QDir::fromNativeSeparators(files.at(i)).replace("//", "/");
                QFileInfo file(buf);
                if (file.exists() && file.isFile()) {
                    files[i] = buf;
                } else {
                    consoleOut.outToConsole(QStringLiteral("The file %1 is corrupted or missing.").arg(buf));
                    return 1;
                }
            }
        }

        if (!searchFolder.isEmpty()) {
            searchFolder = QDir::fromNativeSeparators(searchFolder);
            searchFolder.replace("//", "/");
            if (QDir(searchFolder).exists()) {
                files.append( CDataSourceList::getDataSourceList(searchFolder, QStringList() << "*.csv") );
            } else {
                consoleOut.outToConsole(QStringLiteral("Cannot find the directory."));
                return 1;
            }
        }
        files.removeDuplicates();

        reportName = cmd.reportName();
        if (reportName.isEmpty()) {
            QDateTime now = QDateTime::currentDateTime();
            reportName = QString("%1_report.xlsx").arg(now.toString("ddMMyyyy-hhmmsszzz"));
            __DEBUG( reportName )
        } else {
            if (reportName.indexOf(".xlsx") == -1) {
                reportName = reportName + ".xlsx";
            }
            reportName = QDir::fromNativeSeparators(reportName);
            __DEBUG( reportName )
        }
        if (reportName.indexOf('/') == -1) {
            QString filePath = QFileInfo(files.at(0)).absolutePath();
            reportName = QDir(filePath).filePath(reportName);
            __DEBUG( reportName )
        }
        reportName.replace("//", "/");

        excludedUsers = cmd.excludedUsernames();
        QString buf;
        for (qsizetype i = 0; i < excludedUsers.size(); ++i) {
            buf = excludedUsers.at(i);
            if (!sanitizeValue(buf)) {
                consoleOut.outToConsole(QStringLiteral("Invalid character in the value %1").arg(buf));
                return 1;
            }
        }
        __DEBUG( excludedUsers )
    } //if (argc == 1)

    consoleOut.outToConsole(QStringLiteral("MMS Event Log Conversion Utility starting..."));

    QString iniFile = QStringLiteral("%1.ini").arg(appName);
    if (!ELCSettings::instance().init(appPath, iniFile)) {
        consoleOut.outToConsole(QStringLiteral("Cannot init settings class."));
        return 1;
    }

    //get path to the DB
    ELCSettings &settings = ELCSettings::instance();
    QString dbName =  QDir::fromNativeSeparators(settings.getMain("SETTINGS/db_file_name").toString().trimmed());
    __DEBUG( dbName )
    if (dbName.isEmpty()) {
        consoleOut.outToConsole(QStringLiteral("Cannot get database file name."));
        return 1;
    }
    QString cleardb = settings.getMain("SETTINGS/clear_on_startup").toString().trimmed();
    if (cleardb.isEmpty() || (QString::compare(cleardb, "yes", Qt::CaseInsensitive) == 0)) {
        QString errorString;
        if (!trunvateDB(dbName, errorString)) {
            consoleOut.outToConsole(QStringLiteral("Cannot open database: %1").arg(errorString));
            return 1;
        }
    }

    CSVThreadLoader loader;
    bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)), Qt::DirectConnection);
    Q_ASSERT_X(retVal, "connect", "connection is not established");
    QString internalipFirstOctet = settings.getMain("SETTINGS/internal_ip_start_octet").toString().trimmed();
    QString tempStore = settings.getMain("SETTINGS/temp_store").toString().trimmed();
    QString journalMode = settings.getMain("SETTINGS/journal_mode").toString().trimmed();
    retVal = loader.init(dbName, true, internalipFirstOctet, tempStore, journalMode, "\r\n");
    if (retVal) {
        loader.setFileName(files);
        loader.start();

        consoleOut.outToConsole(QStringLiteral("wait..."));
        loader.wait();
        retVal = loader.getStatus();
    }
    QObject::disconnect(&loader, SIGNAL(sendMessage(QString)), &consoleOut, SLOT(printToConsole(QString)));
    if (retVal) {
        consoleOut.outToConsole(QStringLiteral("File reading finished"));
    } else {
        consoleOut.outToConsole(QStringLiteral("Error reading file: %1").arg(loader.errorString()));
        return 1;
    }

    consoleOut.outToConsole(QStringLiteral("Generate report"));
    CSVThreadReportBuilder report;
    if (report.init(dbName, reportName, excludedUsers)) {
        report.start();

        consoleOut.outToConsole(QStringLiteral("wait..."));
        report.wait();
        retVal = report.getStatus();
    }

    if (retVal) {
        consoleOut.outToConsole(QStringLiteral("Report generating finished.\nThe result in the %1 file.").arg(reportName));
    } else {
        consoleOut.outToConsole(QStringLiteral("Error generate report: %1").arg(report.errorString()));
        return 1;
    }

#ifdef QT_DEBUG
    return 0;
#else
    return a.exec();
#endif

}
