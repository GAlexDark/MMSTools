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
    QString path();
    QStringList files();
    QString reportName();
    QStringList excludedUsernames();
    QStringList includedUsernames();
    void showHelpAndExit();
    RunningMode getRunningMode();

    bool getDataFilesList(QStringList &fileList);
    QString getReportName();
    bool getExcludedUserNames(QStringList &excludedUsersList);
    bool getIncludedUserNames(QStringList &includedUsersList);
    QString errorString() const { return m_errorString; }

private:
    QCommandLineParser  m_parser;

    bool                m_isPath,
                        m_isFiles,
                        m_isReportName,
                        m_isExcluded,
                        m_isIncluded;
    bool                m_isImportOnly,
                        m_isReportOnly,
                        m_isCleanDbOnly;

    QStringList         m_filesList;
    QString             m_errorString;

    bool checkData(const QStringList &data);
};

#endif // QCOMMANDLINEPARSERHELPER_H
