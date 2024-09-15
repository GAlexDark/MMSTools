/****************************************************************************
*
* The class for initializing the settings class instance of
*   the Event Log Conversion Console Utility.
* Copyright (C) 2023-2024  Oleksii Gaienko
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

#include "CElcConsoleAppSettings.h"

CElcConsoleAppSettings& CElcConsoleAppSettings::instance()
{
    static CElcConsoleAppSettings g_elcConsoleSettings;
    return g_elcConsoleSettings;
}

void
CElcConsoleAppSettings::createDefault(const QString& iniPath)
{
    CElcCommonSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QLatin1String("SETTINGS"));
    settings.setValue(QLatin1String("data_has_headers"), QLatin1String("yes"));
    settings.endGroup();
}

bool
CElcConsoleAppSettings::isDataHasHeaders() const
{
    QString buf = getMain(QLatin1String("SETTINGS/data_has_headers")).toString().trimmed();
    return buf.isEmpty() || QString::compare(buf, QLatin1String("yes"), Qt::CaseInsensitive) == 0 ? true : false;
}
