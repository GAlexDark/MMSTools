/****************************************************************************
*
* The class for initializing the settings class instance and
*   creating additional settings for the MMS Event Log Conversion GUI Utility.
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

#ifndef CELCGUIAPPSETTINGS_H
#define CELCGUIAPPSETTINGS_H

#include <CElcCommonSettings.h>

class CElcGuiAppSettings : public CElcCommonSettings
{
public:
    static CElcGuiAppSettings& instance();
    QString getLastDir() const;
#ifdef Q_OS_WIN
    qsizetype getDefaultMonitor() const;
#endif

private:
    CElcGuiAppSettings() = default;
    ~CElcGuiAppSettings() = default;

    CElcGuiAppSettings(CElcGuiAppSettings const&) = delete;
    CElcGuiAppSettings& operator= (CElcGuiAppSettings const&) = delete;

    void createDefault(const QString& iniPath) override;
};

#endif // CELCGUIAPPSETTINGS_H
