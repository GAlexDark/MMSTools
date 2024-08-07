/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  MMS XLSX to JSON Conversion Console Utility
*  MMS XLSX to JSON Conversion Console Utility
*
*  Module name: QCommandLineParserHelper.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing Class.
*
****************************************************************************/

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
