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

#ifndef CBASICSETTINGS_H
#define CBASICSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QScopedPointer>

class CBasicSettings
{
public:
    bool init(const QString& appPath, const QString &fileName, bool isTerminalMode);
    QVariant getMain(const QString& keyName) const;
    void setMain(const QString& group, const QString& keyName, const QVariant& value);
    bool isRdsEnabled() const { return m_isRdsEnabled; }

protected:
    virtual void createDefault(const QString& iniPath) = 0;

private:
    QScopedPointer<QSettings> m_settings;
    bool                      m_isRdsEnabled = false;
};

#endif // CBASICSETTINGS_H
