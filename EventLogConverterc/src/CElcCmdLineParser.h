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

#ifndef CELCCMDLINEPARSER_H
#define CELCCMDLINEPARSER_H

#include <QCoreApplication>
#include <QString>
#include <QStringList>

#include "CBasicCmdLineParser.h"

enum class RunningMode { RUNNINGMODE_DEFAULT, RUNNINGMODE_IMPORT_ONLY, RUNNINGMODE_REPORT_ONLY, RUNNINGMODE_CLEAN_DB };

class CElcCmdLineParser : public CBasicCmdLineParser
{
public:
    explicit CElcCmdLineParser();
    RunningMode getRunningMode() const;

    bool getDataFilesList(QStringList &fileList);
    QString getReportName() const;
    bool getExcludedUserNames(QStringList &excludedUsersList);
    bool getIncludedUserNames(QStringList &includedUsersList);

private:
    QStringList excludedUsernames() const;
    QStringList includedUsernames() const;

    bool                m_isPath = false;
    bool                m_isFiles = false;
    bool                m_isReportName = false;
    bool                m_isExcluded = false;
    bool                m_isIncluded = false;
    bool                m_isImportOnly = false;
    bool                m_isReportOnly = false;
    bool                m_isCleanDbOnly = false;
    QStringList         m_filesList;

    bool checkData(const QStringList &data);
    bool addOption(const QCoreApplication &app) override;
    bool checkOption() override;
};

#endif // CELCCMDLINEPARSER_H
