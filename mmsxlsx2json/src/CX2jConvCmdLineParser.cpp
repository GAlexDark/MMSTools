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

#include "CX2jConvCmdLineParser.h"

#include <QFileInfo>
#include <QDateTime>

bool
CX2jConvCmdLineParser::addOption(const QCoreApplication &app)
{
    const QString importDescription(QLatin1String("The path to the XLSX file for conversion to the JSON. Usage:\n-i file or\n--input file"));
    const QString outputDescription(QLatin1String("The path to the directory and name of the JSON output file. Usege:\n-o file or\n--output file"));
    const QString modeDescription(QLatin1String("This value defines the format of the JSON byte array produced when converting:\nindented - defines a human-readable output,\ncompact - defines a compact output.\nUsage:\n-m indented or\n-m compact"));

    QCommandLineOption importOption(QStringList() << "i" << "input", importDescription, "file");
    bool retVal = m_parser.addOption(importOption);
    if (retVal) {
        QCommandLineOption outputOption(QStringList() << "o" << "output", outputDescription, "file");
        retVal = m_parser.addOption(outputOption);
        if (retVal) {
            QCommandLineOption modeOption(QStringList() << "m" << "mode", modeDescription, "mode");
            retVal = m_parser.addOption(modeOption);
            if (retVal) {
                m_parser.process(app);
                m_isImport = m_parser.isSet(importOption);
                m_isOutput = m_parser.isSet(outputOption);
                m_isMode = m_parser.isSet(modeOption);
            }
        }
    }
    if (!retVal) {
        setErrorString(QLatin1String("The fatal error has occurredd. The program will be closed."));
    }
    return retVal;
}

bool
CX2jConvCmdLineParser::checkOption()
{
    bool retVal = m_isImport && m_isMode;
    if (!retVal) {
        setErrorString(QLatin1String("The <input> or <mode> arguments are missing."));
    }
    return retVal;
}

bool
CX2jConvCmdLineParser::getDataFile(QString &fileName)
{
    bool retVal = m_isImport;
    if (retVal) {
        fileName = m_parser.value("input");
        QFileInfo fi(fileName);
        if (fi.exists() && fi.isFile()) {
            if (QString::compare(fi.suffix(), QLatin1String("xlsx"), Qt::CaseInsensitive) == 0) {
                fileName = fi.absoluteFilePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                m_path = fi.absoluteDir();
                m_baseName = fi.baseName();
            } else {
                setErrorString(QLatin1String("The file %1 has the wrong extension.").arg(fi.fileName()));
                retVal = false;
            }
        } else {
            setErrorString(QLatin1String("The file %1 is corrupted or missing.").arg(fi.fileName()));
            retVal = false;
        }
    }

    return retVal;
}

QString
CX2jConvCmdLineParser::getReportName() const
{
    QString retVal = m_isOutput ? m_parser.value("output") : QLatin1String("%1.json").arg(m_baseName);
    if (!retVal.endsWith(QLatin1String(".json"), Qt::CaseInsensitive)) {
        retVal = retVal + QLatin1String(".json");
    }

    if ((retVal.indexOf('/') == -1) || (retVal.indexOf('\\') == -1)) {
        // if m_filesList.isEmpty() == true, we returns utility directory as path for the report file
        // otherwise - returns path to the first data file
        retVal = m_path.filePath(retVal);
    }
    retVal = QDir::fromNativeSeparators(retVal);
    retVal.replace(QLatin1String("//"), QLatin1String("/"));

    return retVal;
}

OutputMode
CX2jConvCmdLineParser::getOutputMode() const
{
    OutputMode retVal = OutputMode::OUTPUTMODE_COMPACT;
    QString buf = m_parser.value("mode");
    if (QString::compare(buf, QLatin1String("indented"), Qt::CaseInsensitive) == 0) {
        retVal = OutputMode::OUTPUTMODE_INDENTED;
    }
    return retVal;
}
