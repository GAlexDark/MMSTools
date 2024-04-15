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
    void init(bool usePrettySelector = true);
    QStringList getVisibleReportsNames() const { return m_visibleReportsNames; }
    quint16 prettySelector(const quint16 id);

private:
    QStringList m_visibleReportsNames;
    bool        m_usePrettySelector;
};

#endif // CREPORTMANAGER_H
