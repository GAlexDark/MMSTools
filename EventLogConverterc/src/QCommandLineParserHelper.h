#ifndef QCOMMANDLINEPARSERHELPER_H
#define QCOMMANDLINEPARSERHELPER_H

#include <QCoreApplication>
#include <QCommandLineParser>

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

private:
    QCommandLineParser  m_parser;

    bool                m_isPath,
                        m_isFiles,
                        m_isReportName,
                        m_isExcluded,
                        m_isIncluded;

    QStringList         m_filesList;

    void parseValuesList(QStringList &data);
};

#endif // QCOMMANDLINEPARSERHELPER_H
