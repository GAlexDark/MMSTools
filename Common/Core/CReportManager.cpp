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

#include "CReportManager.h"
#include <QMetaClassInfo>
#include <QMap>

#include "CEventLogReport.h"
#include "CAuditTrailReport.h"
#include "CSummaryReport.h"
#include "CSystemLogReport.h"

static CReportManager g_reportManager;

CReportManager& CReportManager::instance()
{
    return g_reportManager;
}

void
CReportManager::init()
{
    qRegisterMetaType<pEventLogReport>("CEventLogReport");
    addClassListItem(QLatin1String("CEventLogReport")); // ID=1
    qRegisterMetaType<pAuditTrailReport>("CAuditTrailReport");
    addClassListItem(QLatin1String("CAuditTrailReport")); // ID=2
    qRegisterMetaType<pSummaryReport>("CSummaryReport");
    addClassListItem(QLatin1String("CSummaryReport")); // ID=1000
    qRegisterMetaType<pSystemLogReport>("CSystemLogReport");
    addClassListItem(QLatin1String("CSystemLogReport")); // ID=3

    QMap<quint16, QString> reportNameMap;
    pBasicReport ptr = nullptr;
    QMetaType type;
    QStringList classList = getClassList();
    for (const QString &name : classList) {
        type = QMetaType::fromName(name.toUtf8());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicReport>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            quint16 id = ptr->reportID();
            m_ids.append(id);
            reportNameMap[id] = ptr->visibleReportName();

            type.destroy(ptr);
            ptr = nullptr;
        }
    }
    std::sort(m_ids.begin(), m_ids.end());
    for (const quint16 i: m_ids) {
        m_visibleReportsNames.append(reportNameMap[i]);
    }
}
