/****************************************************************************
*
* The Event Log Conversion Utility base class for creating default settings
*   in the ini file.
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

#ifndef CELCCOMMONSETTINGS_H
#define CELCCOMMONSETTINGS_H
#include "CBasicSettings.h"

class CElcCommonSettings : public CBasicSettings
{
public:
    QString getDbFileName() const;
    bool isClearDbOnStartup() const;
    QString getInternalIpStartOctet() const;
    QString getSynchronousType() const;
    QString getJournalModeType() const;
    QString getTempStore() const;
    QString getLockingMode() const;
    bool getShowMilliseconds() const;
    QString getAllowedChars() const;

protected:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCCOMMONSETTINGS_H
