/****************************************************************************
*
* The class for the reports management.
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

#ifndef CREPORTMANAGER_H
#define CREPORTMANAGER_H

/**********************************************************
 * ID=1: Event Log parser and Event Log report
 * ID=2: Audit Trail Log parser and Audit Trail Log report
 * ID=3: Summury report
 *********************************************************/

#include "CBasicManager.h"
#include "CBasicReport.h"

class CReportManager : public CBasicManager<pBasicReport>
{
public:
    static CReportManager& instance();
    void init();
    const QStringList& getVisibleReportsNames() const { return m_visibleReportsNames; }

private:
    CReportManager() = default;
    ~CReportManager() = default;

    CReportManager(CReportManager const&) = delete;
    CReportManager& operator= (CReportManager const&) = delete;

    QStringList m_visibleReportsNames;
};

#endif // CREPORTMANAGER_H
