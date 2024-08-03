#ifndef QCOMMANDLINEPARSERHELPER_H
#define QCOMMANDLINEPARSERHELPER_H

#include <QCoreApplication>
#include <QCommandLineParser>

#include <QDir>

enum class OutputMode { OUTPUTMODE_INDENTED, OUTPUTMODE_COMPACT };

namespace xlsxc {
class QCommandLineParserHelper
{
public:
    explicit QCommandLineParserHelper();
    bool parseCmdArgs(const QCoreApplication &app);
    [[noreturn]] void showHelpAndExit();
    bool getDataFile(QString &fileName);
    QString getReportName() const;
    OutputMode getOutputMode() const;

    const QString& errorString() const { return m_errorString; }

private:
    bool addCnvOption(const QCoreApplication &app);
    bool checkCnvOption();

    QCommandLineParser  m_parser;
    bool m_isImport = false;
    bool m_isOutput = false;
    bool m_isMode = false;
    QString m_errorString;
    QDir m_path;
};
} // namespace xlsxconv


#endif // QCOMMANDLINEPARSERHELPER_H
