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

#ifndef CELCCONSOLEAPPSETTINGS_H
#define CELCCONSOLEAPPSETTINGS_H

#include "CElcCommonSettings.h"

class CElcConsoleAppSettings : public CElcCommonSettings
{
public:
    static CElcConsoleAppSettings& instance();
    bool isDataHasHeaders() const;

private:
    CElcConsoleAppSettings() = default;
    ~CElcConsoleAppSettings() = default;

    CElcConsoleAppSettings(CElcConsoleAppSettings const&) = delete;
    CElcConsoleAppSettings& operator= (CElcConsoleAppSettings const&) = delete;

    void createDefault(const QString& iniPath) override;
};

#endif // CELCCONSOLEAPPSETTINGS_H
