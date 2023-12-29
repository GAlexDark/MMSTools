#include "QCommandLineParserHelper.h"

QCommandLineParserHelper::QCommandLineParserHelper(): m_isPath(false), m_isFiles(false), m_isReportName(false), m_isExcluded(false)
{}

bool
QCommandLineParserHelper::parseCmdArgs(const QCoreApplication &app)
{
    m_parser.addHelpOption();

    QCommandLineOption path_option(QStringList() << "p" << "path", "Path to the directory with the MMS Event Log files for the report.", "path");
    bool retVal = m_parser.addOption(path_option);
    if (retVal) {
        QCommandLineOption files_option(QStringList() << "f" << "files", "MMS Event Log data file(s) for the report. Usage:\n-f <file1> -f <file2> ... -f <fileN>" , "file");
        retVal = m_parser.addOption(files_option);
        if (retVal) {
            QCommandLineOption reportname_option(QStringList() << "r" << "report", "Path to the directory and name of the report file.", "file");
            retVal = m_parser.addOption(reportname_option);
            if (retVal) {
                QCommandLineOption exclude_option(QStringList() << "e" << "exclude", "The list of usernames separated by ',' or ';' excluded from the report. \
Usage:\n-e <user1> -e <user2> ... -e <userN> or\n -e <user1,user2,..,userN> or\n-e <user1;user2;..;userN>", "usernames");
                retVal = m_parser.addOption(exclude_option);
                if (retVal) {
                    m_parser.process(app);
                    m_isPath = m_parser.isSet(path_option);
                    m_isFiles = m_parser.isSet(files_option);
                    m_isReportName = m_parser.isSet(reportname_option);
                    m_isExcluded = m_parser.isSet(exclude_option);
                }
            }
        }
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
    if (retVal.size() == 1) {
        QStringList res;
        QString buf = retVal.at(0).trimmed();
        if (buf.indexOf(',') != -1) {
            res.append(buf.split(','));
        }
        if (buf.indexOf(';') != -1) {
            res.append(buf.split(';'));
        }
        qsizetype i = 0;
        while (i < res.size()) {
            buf = res.at(i).trimmed();
            res[i] = buf;
            if (buf.isEmpty() )
                res.removeAt(i);
            else i++;
        }
        retVal.clear();
        retVal.append(res);
    }
    retVal.removeDuplicates();
    return retVal;
}

void
QCommandLineParserHelper::showHelpAndExit()
{
    m_parser.showHelp(0);
}
