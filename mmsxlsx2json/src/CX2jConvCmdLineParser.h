/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  MMS XLSX to JSON Conversion Console Utility
*  MMS XLSX to JSON Conversion Console Utility
*
*  Module name: CX2jConvCmdLineParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing Class.
*
****************************************************************************/

#ifndef CX2JCONVCMDLINEPARSER_H
#define CX2JCONVCMDLINEPARSER_H

#include <QCoreApplication>
#include <QCommandLineParser>

#include <QDir>

enum class OutputMode { OUTPUTMODE_INDENTED, OUTPUTMODE_COMPACT };

class CX2jConvCmdLineParser
{
public:
    explicit CX2jConvCmdLineParser();
    bool parseCmdArgs(const QCoreApplication &app);
    [[noreturn]] void showHelpAndExit();
    bool getDataFile(QString &fileName);
    QString getReportName() const;
    OutputMode getOutputMode() const;

    const QString& errorString() const { return m_errorString; }

private:
    bool addOption(const QCoreApplication &app);
    bool checkOption();

    QCommandLineParser  m_parser;
    bool m_isImport = false;
    bool m_isOutput = false;
    bool m_isMode = false;
    QString m_errorString;
    QDir m_path;
};

#endif // CX2JCONVCMDLINEPARSER_H
