/****************************************************************************
*
* The MMS XLSX to JSON Conversion Console Utility input arguments parsing class.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CX2JCONVCMDLINEPARSER_H
#define CX2JCONVCMDLINEPARSER_H

#include <QCoreApplication>
#include <QDir>
#include <QString>

#include "CBasicCmdLineParser.h"
#include "MMSTypes.h"

enum class OutputMode { OUTPUTMODE_INDENTED, OUTPUTMODE_COMPACT };

class CX2jConvCmdLineParser : public CBasicCmdLineParser
{
public:
    bool getDataFile(QString &fileName);
    QString getReportName() const;
    OutputMode getOutputMode() const;
    bool isSilent() const { return m_isSilent; };

private:
    bool addOption(const QCoreApplication &app) override;
    bool checkOption() override;

    bool m_isImport = false;
    bool m_isOutput = false;
    bool m_isMode = false;
    bool m_isSilent = false;
    QDir m_path;
    QString m_baseName;
};

class X2jConvCmdLineParser : public mms::MmsCommonException
{
public:
    using MmsCommonException::MmsCommonException;
};

#endif // CX2JCONVCMDLINEPARSER_H
