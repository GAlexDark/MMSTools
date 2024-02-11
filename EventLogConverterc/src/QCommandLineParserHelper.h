#ifndef QCOMMANDLINEPARSERHELPER_H
#define QCOMMANDLINEPARSERHELPER_H

#include <QCoreApplication>
#include <QCommandLineParser>

enum RunningMode { RUNNINGMODE_DEFAULT, RUNNINGMODE_IMPORT_ONLY, RUNNINGMODE_REPORT_ONLY };

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
                        m_isReportOnly;

    QStringList         m_filesList;
    QString             m_errorString;

    bool checkData(const QStringList &data);
};

#endif // QCOMMANDLINEPARSERHELPER_H
