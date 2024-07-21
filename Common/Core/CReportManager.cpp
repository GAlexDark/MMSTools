/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CReportManager.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for report management
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
    m_classList.append(QLatin1String("CEventLogReport")); // ID=1
    qRegisterMetaType<pAuditTrailReport>("CAuditTrailReport");
    m_classList.append(QLatin1String("CAuditTrailReport")); // ID=2
    qRegisterMetaType<pSummaryReport>("CSummaryReport");
    m_classList.append(QLatin1String("CSummaryReport")); // ID=1000
    qRegisterMetaType<pSystemLogReport>("CSystemLogReport");
    m_classList.append(QLatin1String("CSystemLogReport")); // ID=3

    QMap<quint16, QString> reportNameMap;
    pBasicReport ptr = nullptr;
    QMetaType type;
    for (const QString &name : m_classList) {
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
