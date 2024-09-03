/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicCmdLineParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Basic Class for parsing input arguments of console utilities.
*
****************************************************************************/

#ifndef CBASICCMDLINEPARSER_H
#define CBASICCMDLINEPARSER_H

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>

class CBasicCmdLineParser
{
public:
    explicit CBasicCmdLineParser();
    bool parseCmdArgs(const QCoreApplication &app);
    [[noreturn]] void showHelpAndExit();
    const QString& errorString() const { return m_errorString; }

private:
    virtual bool addOption(const QCoreApplication &app) = 0;
    virtual bool checkOption() = 0;

    QString m_errorString;

protected:
    void setErrorString(const QString &errorString) { m_errorString = errorString; }

    QCommandLineParser  m_parser;
};

#endif // CBASICCMDLINEPARSER_H
