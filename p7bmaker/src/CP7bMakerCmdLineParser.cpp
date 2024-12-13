/****************************************************************************
*
* The p7b file maker Console Utility input arguments parsing class.
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

#include "CP7bMakerCmdLineParser.h"

#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>

#include "elcUtils.h"

bool
CP7bMakerCmdLineParser::addOption(const QCoreApplication &app)
{
    const QString storeDescription(QLatin1String("The folder path contains one *.p7b file and several *.cer, *.crt, or *.der files to add."));
    const QString silentDescription(QLatin1String("Silent mode"));
    const QString outputFormatDescription(QLatin1String("Outpur format: ASN1 or PEM (ASN1 by default)"));

    QCommandLineOption storeOption(QStringList() << "l" << "localstore", storeDescription, "path");
    bool retVal = m_parser.addOption(storeOption);
    if (retVal) {
        QCommandLineOption silentOption(QStringList() << "silent", silentDescription);
        retVal = m_parser.addOption(silentOption);
        if (retVal) {
            QCommandLineOption outputFormatOption(QStringList() << "o" << "output", outputFormatDescription, "format");
            retVal = m_parser.addOption(outputFormatOption);
            if (retVal) {
                m_parser.process(app);
                m_isStore = m_parser.isSet(storeOption);
                m_isSilent = m_parser.isSet(silentOption);
                m_isOutputFormat = m_parser.isSet(outputFormatOption);
            }
        }
    }
    if (!retVal) {
        setErrorString(QLatin1String("The fatal error has occurredd. The program will be closed."));
    }
    return retVal;
}

bool
CP7bMakerCmdLineParser::checkOption()
{
    if (!m_isStore) {
        setErrorString(QLatin1String("The <store> argument is missing."));
    }
    return m_isStore;
}

bool
CP7bMakerCmdLineParser::getP7bFileName(QStringList &p7bFileName)
{
    bool retVal = true;
    if (m_isStore) {
        m_searchFolder = m_parser.value("localstore").trimmed();
        QDir dir(m_searchFolder);
        if (dir.exists()) {
            m_isSearchFolderExists = true;
            m_searchFolder = dir.absolutePath();
            QString mask = QLatin1String("*.p7b");
            p7bFileName.append( elcUtils::getDataSourceList(m_searchFolder, QStringList() << mask) );
        } else {
            setErrorString(QStringLiteral("Cannot find the directory %1.").arg(m_searchFolder));
            retVal = false;
        }
    }

    if (retVal) {
        p7bFileName.removeDuplicates();
    }
    return retVal;
}

bool
CP7bMakerCmdLineParser::getCertsList(QStringList &certsList)
{
    bool retVal = true;
    if (m_isSearchFolderExists) {
        QStringList masks = { "*.cer", "*.crt", "*.der" };
        for (const QString &maskItem : masks) {
            certsList.append( elcUtils::getDataSourceList(m_searchFolder, QStringList() << maskItem) );
        }
    } else {
        setErrorString(QStringLiteral("Cannot find the directory %1.").arg(m_searchFolder));
        retVal = false;
    }

    if (!retVal) {
        certsList.removeDuplicates();
    }
    return retVal;
}

QString
CP7bMakerCmdLineParser::getOutputFormat()
{
    QString retVal;
    if (m_isOutputFormat) {
        retVal = m_parser.value("output").trimmed().toUpper();
        if (QString::compare(retVal, "PEM", Qt::CaseInsensitive) != 0) {
            retVal = "ASN1";
        }
    } else {
        retVal = "ASN1";
    }
    return retVal;
}
