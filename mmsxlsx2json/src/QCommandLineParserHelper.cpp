/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  MMS XLSX to JSON Conversion Console Utility
*  MMS XLSX to JSON Conversion Console Utility
*
*  Module name: QCommandLineParserHelper.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing Class.
*
****************************************************************************/

#include "QCommandLineParserHelper.h"

#include <QFileInfo>

bool
xlsxc::QCommandLineParserHelper::addCnvOption(const QCoreApplication &app)
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
        m_errorString = QLatin1String("The fatal error has occurredd. The program will be closed.");
    }
    return retVal;
}

bool
xlsxc::QCommandLineParserHelper::checkCnvOption()
{
    bool retVal = m_isImport && m_isMode;
    if (!retVal) {
        m_errorString = QLatin1String("The <input> or <mode> arguments are missing.");
    }
    return retVal;
}

xlsxc::QCommandLineParserHelper::QCommandLineParserHelper()
{
    m_errorString.clear();
}

bool
xlsxc::QCommandLineParserHelper::parseCmdArgs(const QCoreApplication &app)
{
    m_parser.addHelpOption();

    bool retVal = addCnvOption(app);
    if (retVal) {
        retVal = checkCnvOption();
    }

    return retVal;
}

[[noreturn]] void
xlsxc::QCommandLineParserHelper::showHelpAndExit()
{
    m_parser.showHelp(0);
}

bool
xlsxc::QCommandLineParserHelper::getDataFile(QString &fileName)
{
    bool retVal = m_isImport;
    if (retVal) {
        fileName = m_parser.value("input");
        QFileInfo fi(fileName);
        if (fi.exists() && fi.isFile()) {
            if (QString::compare(fi.suffix(), QLatin1String("xlsx"), Qt::CaseInsensitive) == 0) {
                fileName = fi.absoluteFilePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
                m_path = fi.absoluteDir();
            } else {
                m_errorString = QLatin1String("The file %1 has the wrong extension.").arg(fi.fileName());
                retVal = false;
            }
        } else {
            m_errorString = QLatin1String("The file %1 is corrupted or missing.").arg(fi.fileName());
            retVal = false;
        }
    }

    return retVal;
}

QString
xlsxc::QCommandLineParserHelper::getReportName() const
{
    QString retVal;
    if (m_isOutput) {
        retVal = m_parser.value("output");
    } else {
        QDateTime now = QDateTime::currentDateTime();
        retVal = QLatin1String("%1.json").arg(now.toString(QLatin1String("ddMMyyyy-hhmmsszzz")));
    }

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
xlsxc::QCommandLineParserHelper::getOutputMode() const
{
    OutputMode retVal = OutputMode::OUTPUTMODE_COMPACT;
    QString buf = m_parser.value("mode");
    if (QString::compare(buf, QLatin1String("indented"), Qt::CaseInsensitive) == 0) {
        retVal = OutputMode::OUTPUTMODE_INDENTED;
    }
    return retVal;
}
