/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CElcCmdLineParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing class.
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
