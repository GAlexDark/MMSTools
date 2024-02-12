#include "QCommandLineParserHelper.h"
#include <QFileInfo>
#include <QDir>

#include "elcUtils.h"

bool
QCommandLineParserHelper::checkData(const QStringList &data)
{
    QString buf;
    bool retVal = true;
    for (qsizetype i = 0; i < data.size(); ++i) {
        buf = data.at(i);
        if (!elcUtils::sanitizeValue(buf)) {
            m_errorString =
                QStringLiteral("Invalid character in the value %1").arg(buf);
            retVal = false;
        }
    }
    return retVal;
}

QCommandLineParserHelper::QCommandLineParserHelper()
    : m_isPath(false), m_isFiles(false), m_isReportName(false), m_isExcluded(false), m_isIncluded(false),
    m_isImportOnly(false), m_isReportOnly(false)
{
    m_filesList.clear();
}

bool
QCommandLineParserHelper::parseCmdArgs(const QCoreApplication &app)
{
    m_parser.addHelpOption();

    const QString importOnlyDescription = QStringLiteral("The utility starts in the import data-only mode without report generation. \
In this mode cleaning the database on startup will be ignored. When using this option, the report options are ignored.");
    const QString reportOnlyDescription = QStringLiteral("The utility starts in the generation report-only mode without importing data. \
In this mode cleaning the database on startup will be ignored. When using this option, the import options are ignored.\n \
If these options are not specified, data will be imported and the report generated.");
    const QString cleanDbOnlyDescription = QStringLiteral("The utility starts in the clear database-only mode without import data and report generation.");
    const QString pathDescription = QStringLiteral("Path to the directory with the MMS Event Log files for the report.");
    const QString filesDescription = QStringLiteral("MMS Event Log data file(s) for the report. Usage:\n-f file1 -f file2 ... -f fileN");
    const QString reportNameDescription = QStringLiteral("Path to the directory and name of the report file.");
    const QString excludeDescription = QStringLiteral("The list of usernames separated by ',' or ';' excluded from the report. \
Usage:\n-e user1 -e user2 ... -e userN or\n -e user1,user2,..,userN or\n-e user1;user2;..;userN");
    const QString includeDescription = QStringLiteral("The list of usernames separated by ',' or ';' only included from the report. \
When using this option, the exclude option is ignored. \
Usage:\n-i user1 -i user2 ... -i userN or\n -i user1,user2,..,userN or\n-i user1;user2;..;userN");

    QCommandLineOption importOnlyOption(QStringList() << "importonly", importOnlyDescription);
    bool retVal = m_parser.addOption(importOnlyOption);
    if (retVal) {
        QCommandLineOption reportOnlyOption(QStringList() << "reportonly", reportOnlyDescription);
        retVal = m_parser.addOption(reportOnlyOption);
        if (retVal) {
            QCommandLineOption cleanDbOnlyOption(QStringList() << "cleandb", cleanDbOnlyDescription);
            retVal = m_parser.addOption(cleanDbOnlyOption);
            if (retVal) {
                QCommandLineOption pathOption(QStringList() << "p" << "path", pathDescription, "path");
                retVal = m_parser.addOption(pathOption);
                if (retVal) {
                    QCommandLineOption filesOption(QStringList() << "f" << "files", filesDescription, "files");
                    retVal = m_parser.addOption(filesOption);
                    if (retVal) {
                        QCommandLineOption reportNameOption(QStringList() << "r" << "report", reportNameDescription, "reportname");
                        retVal = m_parser.addOption(reportNameOption);
                        if (retVal) {
                            QCommandLineOption excludeOption(QStringList() << "e" << "exclude", excludeDescription, "usernames");
                            retVal = m_parser.addOption(excludeOption);
                            if (retVal) {
                                QCommandLineOption includeOption(QStringList() << "i" << "include", includeDescription, "usernames");
                                retVal = m_parser.addOption(includeOption);
                                if (retVal) {
                                    m_parser.process(app);
                                    m_isImportOnly = m_parser.isSet(importOnlyOption);
                                    m_isReportOnly = m_parser.isSet(reportOnlyOption);
                                    m_isCleanDbOnly = m_parser.isSet(cleanDbOnlyOption);
                                    m_isPath = m_parser.isSet(pathOption);
                                    m_isFiles = m_parser.isSet(filesOption);
                                    m_isReportName = m_parser.isSet(reportNameOption);
                                    m_isExcluded = m_parser.isSet(excludeOption);
                                    m_isIncluded = m_parser.isSet(includeOption);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (retVal) {
        const QString error1 = QStringLiteral("The '%1' and '%2' options cannot be specified at the same time.");
        bool comb1 = m_isImportOnly && m_isReportOnly && m_isCleanDbOnly;
        bool comb2 = m_isImportOnly && m_isReportOnly;
        bool comb3 = m_isImportOnly && m_isCleanDbOnly;
        bool comb4 = m_isReportOnly && m_isCleanDbOnly;

        if (comb1 || comb2 || comb3 || comb4) {
            m_errorString = error1.arg("--cleandb', '--importonly", "--reportonly");
            retVal = false;
        }
        if (retVal && m_isExcluded && m_isIncluded) {
            m_errorString = error1.arg("--exclude", "--include");
            retVal = false;
        }
    } else {
        m_errorString = QStringLiteral("The fatal error has occurredd. The program will be closed.");
    }

    return retVal;
}

QString
QCommandLineParserHelper::path()
{
    return (m_isPath)? m_parser.value("path") : QString();
}

QStringList
QCommandLineParserHelper::files()
{
    return (m_isFiles)? m_parser.values("files") : QStringList();
}

QString
QCommandLineParserHelper::reportName()
{
    return (m_isReportName)? m_parser.value("report") : QString();
}

QStringList
QCommandLineParserHelper::excludedUsernames()
{
    QStringList retVal = (m_isExcluded)? m_parser.values("exclude") : QStringList();
    elcUtils::parseValuesList(retVal);
    return retVal;
}

QStringList QCommandLineParserHelper::includedUsernames()
{
    QStringList retVal = (m_isIncluded)? m_parser.values("include") : QStringList();
    elcUtils::parseValuesList(retVal);
    return retVal;
}

void
QCommandLineParserHelper::showHelpAndExit()
{
    m_parser.showHelp(0);
}

RunningMode
QCommandLineParserHelper::getRunningMode()
{
    RunningMode retVal = RUNNINGMODE_DEFAULT;
    if (m_isCleanDbOnly) {
        retVal = RUNNINGMODE_CLEAN_DB;
    } else {
        if (m_isImportOnly) {
            retVal = RUNNINGMODE_IMPORT_ONLY;
        } else {
            if (m_isReportOnly) {
                retVal = RUNNINGMODE_REPORT_ONLY;
            }
        }
    }
    return retVal;
}

bool
QCommandLineParserHelper::getDataFilesList(QStringList &fileList)
{
    QString searchFolder(path());
    fileList.append(files());
    bool retVal = true;
    bool isSearchFolderEmpty = searchFolder.isEmpty();

    if (isSearchFolderEmpty && fileList.isEmpty()) {
        m_errorString = QStringLiteral("The <path> and <files> arguments are missing.");
        retVal = false;
    } else {
        if (!fileList.isEmpty()) {
            QFileInfo fi;
            for (qsizetype i = 0; i < fileList.size(); ++i) {
                fi.setFile(fileList.at(i));
                if (fi.exists() && fi.isFile()) {
                    fileList[i] = fi.absoluteFilePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                } else {
                    m_errorString = QStringLiteral("The file %1 is corrupted or missing.").arg(fi.fileName());
                    retVal = false;
                }
            }
        }

        if (!isSearchFolderEmpty && retVal) {
            QFileInfo sf(searchFolder);
            QDir dir = sf.absoluteDir();
            QString mask = sf.fileName().trimmed();
            if (mask.isEmpty()) {
                mask = QStringLiteral("*.csv"); // default mask
            }
            if (dir.exists()) {
                searchFolder = dir.absolutePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                fileList.append( elcUtils::getDataSourceList(searchFolder, QStringList() << mask) );
            } else {
                m_errorString = QStringLiteral("Cannot find the directory %1.").arg(searchFolder);
                retVal = false;
            }
        }
    }
    if (retVal) {
        fileList.removeDuplicates();
        m_filesList.clear();
        m_filesList.append(fileList);
    }

    return retVal;
}

QString
QCommandLineParserHelper::getReportName()
{
    QString retVal = reportName();
    if (retVal.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        retVal = QStringLiteral("%1_report.xlsx").arg(now.toString(QStringLiteral("ddMMyyyy-hhmmsszzz")));
    }
    if (!retVal.endsWith(QLatin1String(".xlsx"), Qt::CaseInsensitive)) {
        retVal = retVal + QStringLiteral(".xlsx");
    }

    if ((retVal.indexOf('/') == -1) || (retVal.indexOf('\\') == -1)) {
        // if m_filesList.isEmpty() == true, we returns utility directory as path for the report file
        // otherwise - returns path to the first data file
        QString filePath = (m_filesList.isEmpty())? QFileInfo(retVal).absolutePath() : QFileInfo(m_filesList.at(0)).absolutePath();
        retVal = QDir(filePath).filePath(retVal);
    }
    retVal = QDir::fromNativeSeparators(retVal);
    retVal.replace(QLatin1String("//"), QLatin1String("/"));

    return retVal;
}

bool
QCommandLineParserHelper::getExcludedUserNames(QStringList &excludedUsersList)
{
    excludedUsersList = excludedUsernames();
    return checkData(excludedUsersList);
}

bool
QCommandLineParserHelper::getIncludedUserNames(QStringList &includedUsersList)
{
    includedUsersList = includedUsernames();
    return checkData(includedUsersList);
}
