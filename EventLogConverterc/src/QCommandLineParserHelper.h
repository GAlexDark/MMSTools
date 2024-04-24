/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: QCommandLineParserHelper.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing class.
*
****************************************************************************/

#ifndef QCOMMANDLINEPARSERHELPER_H
#define QCOMMANDLINEPARSERHELPER_H

#include <QCoreApplication>
#include <QCommandLineParser>

enum RunningMode { RUNNINGMODE_DEFAULT, RUNNINGMODE_IMPORT_ONLY, RUNNINGMODE_REPORT_ONLY, RUNNINGMODE_CLEAN_DB };

class QCommandLineParserHelper
{
public:
    explicit QCommandLineParserHelper();

    bool parseCmdArgs(const QCoreApplication &app);

    void showHelpAndExit();
    RunningMode getRunningMode();

    bool getDataFilesList(QStringList &fileList);
    QString getReportName();
    bool getExcludedUserNames(QStringList &excludedUsersList);
    bool getIncludedUserNames(QStringList &includedUsersList);
    QString errorString() const { return m_errorString; }

private:
    QStringList excludedUsernames();
    QStringList includedUsernames();

    QCommandLineParser  m_parser;

    bool                m_isPath = false;
    bool                m_isFiles = false;
    bool                m_isReportName = false;
    bool                m_isExcluded = false;
    bool                m_isIncluded = false;
    bool                m_isImportOnly = false;
    bool                m_isReportOnly = false;
    bool                m_isCleanDbOnly = false;

    QStringList         m_filesList;
    QString             m_errorString;

    bool checkData(const QStringList &data);
};

#endif // QCOMMANDLINEPARSERHELPER_H
