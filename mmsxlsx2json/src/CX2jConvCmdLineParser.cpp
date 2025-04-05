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
    try {
        const QString importDescription(QLatin1String("The path to the XLSX file for conversion to the JSON. Usage:\n-i file or\n--input file"));
        QCommandLineOption importOption(QStringList() << "i" << "input", importDescription, "file");
        if (!m_parser.addOption(importOption)) {
            throw X2jConvCmdLineParser(QLatin1String("The fatal error has occurred. The program will be closed."));
        }

        const QString outputDescription(QLatin1String("The path to the directory and name of the JSON output file. Usage:\n-o file or\n--output file"));
        QCommandLineOption outputOption(QStringList() << "o" << "output", outputDescription, "file");
        if (!m_parser.addOption(outputOption)) {
            throw X2jConvCmdLineParser(QLatin1String("The fatal error has occurred. The program will be closed."));
        }

        const QString modeDescription(QLatin1String("This value defines the format of the JSON byte array produced when converting:\nindented - defines a human-readable output,\ncompact - defines a compact output.\nUsage:\n-m indented or\n-m compact"));
        QCommandLineOption modeOption(QStringList() << "m" << "mode", modeDescription, "mode");
        if (!m_parser.addOption(modeOption)) {
            throw X2jConvCmdLineParser(QLatin1String("The fatal error has occurred. The program will be closed."));
        }

        const QString silentDescription(QLatin1String("Silent mode"));
        QCommandLineOption silentOption(QStringList() << "silent", silentDescription);
        if (!m_parser.addOption(silentOption)) {
            throw X2jConvCmdLineParser(QLatin1String("The fatal error has occurred. The program will be closed."));
        }

        m_parser.process(app);
        m_isImport = m_parser.isSet(importOption);
        m_isOutput = m_parser.isSet(outputOption);
        m_isMode = m_parser.isSet(modeOption);
        m_isSilent = m_parser.isSet(silentOption);
    } catch (const X2jConvCmdLineParser &ex) {
        setErrorString(ex.what());
        return false;
    }
    return true;
}

bool
CX2jConvCmdLineParser::checkOption()
{
    if (!m_isImport && m_isMode) {
        setErrorString(QLatin1String("The <input> or <mode> arguments are missing."));
        return false;
    }
    return true;
}

bool
CX2jConvCmdLineParser::getDataFile(QString &fileName)
{
    if (!m_isImport) {
        return false;
    }
    fileName = m_parser.value("input");
    QFileInfo fi(fileName);
    if (!fi.exists() || !fi.isFile()) {
        setErrorString(QLatin1String("The file %1 is corrupted or missing.").arg(fi.fileName()));
        return false;
    }

    if (QString::compare(fi.suffix(), QLatin1String("xlsx"), Qt::CaseInsensitive) != 0) {
        setErrorString(QLatin1String("The file %1 has the wrong extension.").arg(fi.fileName()));
        return false;
    }

    fileName = fi.absoluteFilePath(); // The QFileInfo class converts '\\', '//' into '/' in the filepath
    m_path = fi.absoluteDir();
    m_baseName = fi.baseName();

    return true;
}

QString
CX2jConvCmdLineParser::getReportName() const
{
    QString retVal = m_isOutput ? m_parser.value("output") : QLatin1String("%1.json").arg(m_baseName);
    if (!retVal.endsWith(QLatin1String(".json"), Qt::CaseInsensitive)) {
        retVal += QLatin1String(".json");
    }
    if ((retVal.indexOf('/') == -1) && (retVal.indexOf('\\') == -1)) {
        retVal = m_path.filePath(retVal);
    } else {
        retVal = QDir::fromNativeSeparators(retVal);
        retVal.replace(QLatin1String("//"), QLatin1String("/"));
    }
    return retVal;
}

OutputMode
CX2jConvCmdLineParser::getOutputMode() const
{
    QString buf = m_parser.value("mode");
    if (QString::compare(buf, QLatin1String("indented"), Qt::CaseInsensitive) == 0) {
        return OutputMode::OUTPUTMODE_INDENTED;
    }
    return OutputMode::OUTPUTMODE_COMPACT;
}
