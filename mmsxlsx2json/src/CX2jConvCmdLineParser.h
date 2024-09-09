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
#include <QDir>
#include <QString>

#include "CBasicCmdLineParser.h"

enum class OutputMode { OUTPUTMODE_INDENTED, OUTPUTMODE_COMPACT };

class CX2jConvCmdLineParser : public CBasicCmdLineParser
{
public:
    bool getDataFile(QString &fileName);
    QString getReportName() const;
    OutputMode getOutputMode() const;

private:
    bool addOption(const QCoreApplication &app) override;
    bool checkOption() override;

    bool m_isImport = false;
    bool m_isOutput = false;
    bool m_isMode = false;
    QDir m_path;
    QString m_baseName;
};

#endif // CX2JCONVCMDLINEPARSER_H
