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

#include "CEventLogReport.h"
#include "CAuditTrailReport.h"
#include "CSummaryReport.h"

static CReportManager g_reportManager;

CReportManager& CReportManager::instance()
{
    return g_reportManager;
}

void
CReportManager::init(bool usePrettySelector)
{
    m_usePrettySelector = usePrettySelector;
    m_classList.clear();
    qRegisterMetaType<pEventLogReport>("CEventLogReport");
    m_classList.append(QLatin1String("CEventLogReport")); // ID=1, index=0
    qRegisterMetaType<pAuditTrailReport>("CAuditTrailReport");
    m_classList.append(QLatin1String("CAuditTrailReport")); // ID=2, index=1
    qRegisterMetaType<pSummaryReport>("CSummaryReport");
    m_classList.append(QLatin1String("CSummaryReport")); // ID=3, index=2

    m_visibleReportsNames.resize(m_classList.size());

    QString name;
    pBasicReport ptr = nullptr;
    QMetaType type;
    for (qsizetype i = 0; i < m_classList.size(); i++) {
        name = m_classList.at(i).trimmed();
        type = QMetaType::fromName(name.toUtf8());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicReport>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            if (ptr != nullptr) {
                quint16 id = ptr->reportID();
                m_ids.append(id);
                m_visibleReportsNames[id - 1] = ptr->visibleReportName();

                type.destroy(ptr);
                ptr = nullptr;
            }
        }
    } //for
}

/***************************************************************************
 *       -= Parser =-            |       -= Report =-            |  pretty
 * -------------------------------------------------------------------------
 * ID=1: Event Log parser        | ID=1: Event Log report        |
 * ID=2: Audit Trail Log parser  | ID=2: Audit Trail Log report  |
 *                               | ID=3: Summury report          |
 * ID=3: Other Log parser        | ID=4: Other report            |  id++
 **************************************************************************/
quint16
CReportManager::prettySelector(const quint16 id)
{
    quint16 retVal = id;
    if (m_usePrettySelector) {
        if (retVal >= 3) {
            retVal++;
        }
/*************************************************
        // Other verifications must be here
        // if (conditions(retVal)) {
            ...
        }
*************************************************/

    } // m_usePrettySelector

    return retVal;
}
