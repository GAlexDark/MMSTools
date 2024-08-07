/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CReportManager.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for report management
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
    QStringList m_visibleReportsNames;
};

#endif // CREPORTMANAGER_H
