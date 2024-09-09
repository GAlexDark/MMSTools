/****************************************************************************
*
* The Base class for reading and writing settings from an INI file.
* Copyright (C) 2017-2024  Oleksii Gaienko
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

#include "CBasicSettings.h"
#include <QDir>
#include <QFileInfo>

bool
CBasicSettings::init(const QString& appPath, const QString &fileName, bool isTerminalMode)
{
    m_isRdsEnabled = isTerminalMode;
    QString iniFileName = QDir(appPath).filePath(fileName);

    QFileInfo fileInfo(iniFileName);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        createDefault(iniFileName);
    }

    bool retVal = true;
    m_settings.reset(new QSettings(iniFileName, QSettings::IniFormat));
    Q_CHECK_PTR(m_settings);
    if (!m_settings) {
        retVal = false;
    }
    return retVal;
}

QVariant
CBasicSettings::getMain(const QString& keyName) const
{
    return m_settings ? m_settings->value(keyName) : QVariant();
}

void
CBasicSettings::setMain(const QString& group, const QString& keyName, const QVariant& value)
{
    if (m_settings) {
        m_settings->beginGroup(group);
        m_settings->setValue(keyName, value);
        m_settings->endGroup();
    }
}
