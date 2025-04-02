/****************************************************************************
*
* The Event Log Conversion Console Utility input arguments parsing class.
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

#include "CElcCmdLineParser.h"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpressionValidator>
#include "elcUtils.h"
#include "CElcConsoleAppSettings.h"

bool
CElcCmdLineParser::checkData(const QStringList &data)
{
    const CElcConsoleAppSettings &settings = CElcConsoleAppSettings::instance();
    QString defChars = settings.getAllowedChars();
    QRegularExpression mask(QStringLiteral("^([%1,;]+)$").arg(defChars));
    QRegularExpressionValidator validator(mask, nullptr);
    int pos = 0;
    QString buf = data.join(';');
    if (validator.validate(buf, pos) == QValidator::Invalid) {
        setErrorString(QStringLiteral("Invalid character(s) in the usernames: %1\nDefault allowed characters in the usernames: a..z, A..Z, 0..9 and _ (underscore).").arg(buf));
        return false;
    }

    return true;
}

CElcCmdLineParser::CElcCmdLineParser()
{
    m_filesList.clear();
}

bool
CElcCmdLineParser::addOption(const QCoreApplication &app)
{
    const QString importOnlyDescription(QLatin1String("The utility starts in the import data-only mode without report generation. In this mode cleaning the database on startup will be ignored. When using this option, the report options are ignored."));
    const QString reportOnlyDescription(QLatin1String("The utility starts in the generation report-only mode without importing data. In this mode cleaning the database on startup will be ignored. When using this option, the import options are ignored.\nIf these options are not specified, data will be imported and the report generated."));
    const QString cleanDbOnlyDescription(QLatin1String("The utility starts in the clean database-only mode without import data and report generation."));
    const QString pathDescription(QLatin1String("Path to the directory with the MMS Event Log files for the report."));
    const QString filesDescription(QLatin1String("MMS Event Log data file(s) for the report. Usage:\n-f file1 -f file2 ... -f fileN"));
    const QString reportNameDescription(QLatin1String("Path to the directory and name of the report file."));
    const QString excludeDescription(QLatin1String("The list of usernames separated by ',' or ';' excluded from the report. Usage:\n-e user1 -e user2 ... -e userN or\n -e user1,user2,..,userN or\n-e user1;user2;..;userN"));
    const QString includeDescription(QLatin1String("The list of usernames separated by ',' or ';' only included from the report. When using this option, the exclude option is ignored. Usage:\n-i user1 -i user2 ... -i userN or\n -i user1,user2,..,userN or\n-i user1;user2;..;userN"));

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
    if (!retVal) {
        setErrorString(QStringLiteral("The fatal error has occurredd. The program will be closed."));
    }

  return retVal;
}

bool
CElcCmdLineParser::checkOption()
{
    const QString error1 = QLatin1String("The '%1' and '%2' options cannot be specified at the same time.");
    bool comb1 = m_isImportOnly && m_isReportOnly && m_isCleanDbOnly;
    bool comb2 = m_isImportOnly && m_isReportOnly;
    bool comb3 = m_isImportOnly && m_isCleanDbOnly;
    bool comb4 = m_isReportOnly && m_isCleanDbOnly;

    if (comb1 || comb2 || comb3 || comb4) {
        setErrorString(error1.arg("--cleandb', '--importonly", "--reportonly"));
        return false;
    }
    if (m_isExcluded && m_isIncluded) {
        setErrorString(error1.arg("--exclude", "--include"));
        return false;
    }
    if (!m_isPath && !m_isFiles) {
        setErrorString(QStringLiteral("The <path> and <files> arguments are missing."));
        return false;
    }
    return true;
}

QStringList
CElcCmdLineParser::excludedUsernames() const
{
    QStringList usernames = m_isExcluded ? m_parser.values("exclude") : QStringList();
    elcUtils::parseValuesList(usernames);
    return usernames;
}

QStringList
CElcCmdLineParser::includedUsernames() const
{
    QStringList usernames = m_isIncluded ? m_parser.values("include") : QStringList();
    elcUtils::parseValuesList(usernames);
    return usernames;
}

RunningMode
CElcCmdLineParser::getRunningMode() const
{
    if (m_isCleanDbOnly) {
        return RunningMode::RUNNINGMODE_CLEAN_DB;
    }
    if (m_isImportOnly) {
        return RunningMode::RUNNINGMODE_IMPORT_ONLY;
    }
    if (m_isReportOnly) {
        return RunningMode::RUNNINGMODE_REPORT_ONLY;
    }
    return RunningMode::RUNNINGMODE_DEFAULT;
}

bool
CElcCmdLineParser::getDataFilesList(QStringList &fileList)
{
    if (m_isFiles) {
        fileList.append(m_parser.values("files"));
        QFileInfo fileInfo;
        for (QString &item : fileList) {
            fileInfo.setFile(item);
            if (fileInfo.exists() && fileInfo.isFile()) {
                item = fileInfo.absoluteFilePath(); // The QFileInfo class converts '\\', '//' into '/' in the filepath
            } else {
                setErrorString(QStringLiteral("The file %1 is corrupted or missing.").arg(fileInfo.fileName()));
                return false;
            }
        }
    }
    if (m_isPath) {
        QString searchFolder = m_parser.value("path").trimmed();
        QFileInfo sf(searchFolder);
        QDir dir = sf.absoluteDir();
        if (!dir.exists()) {
            setErrorString(QStringLiteral("Cannot find the directory %1.").arg(searchFolder));
            return false;
        }
        searchFolder = dir.absolutePath(); // The QFileInfo class converts '\\', '//' into '/' in the filepath
        QString mask = sf.fileName().trimmed().isEmpty() ? QLatin1String("*.csv") : sf.fileName().trimmed();
        fileList.append(elcUtils::getDataSourceList(searchFolder, QStringList() << mask));
    }

    if (!fileList.isEmpty()) {
        fileList.removeDuplicates();
        m_filesList = fileList;
    }

    return true;
}

QString
CElcCmdLineParser::getReportName() const
{
    QString reportName;
    if (m_isReportName) {
        reportName = m_parser.value("report").trimmed();
        if (reportName.endsWith(QLatin1String(".xls"), Qt::CaseInsensitive)) {
            reportName = reportName.toLower() + 'x';
        } else if (!reportName.endsWith(QLatin1String(".xlsx"), Qt::CaseInsensitive)) {
            reportName += QLatin1String(".xlsx");
        }
    } else {
        reportName = QLatin1String("%1_report.xlsx").arg(QDateTime::currentDateTime().toString(QLatin1String("ddMMyyyy-hhmmsszzz")));
    }

    if (reportName.indexOf('/') == -1 &&
        reportName.indexOf('\\') == -1) {
        QString filePath = m_filesList.isEmpty() ? QFileInfo(reportName).absolutePath() : QFileInfo(m_filesList.at(0)).absolutePath();
        reportName = QDir(filePath).filePath(reportName);
    } else {
        reportName = QDir::fromNativeSeparators(reportName);
        reportName.replace(QLatin1String("//"), QLatin1String("/"));
    }

    return reportName;
}

bool
CElcCmdLineParser::getExcludedUserNames(QStringList &excludedUsersList)
{
    excludedUsersList = excludedUsernames();
    return checkData(excludedUsersList);
}

bool
CElcCmdLineParser::getIncludedUserNames(QStringList &includedUsersList)
{
    includedUsersList = includedUsernames();
    return checkData(includedUsersList);
}
