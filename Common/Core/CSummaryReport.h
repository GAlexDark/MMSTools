/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSummaryReport.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build MMS Event Logs + Audit Trail logs report.
*
****************************************************************************/

#ifndef CSUMMARYREPORT_H
#define CSUMMARYREPORT_H

#include "CBasicReport.h"

class CSummaryReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "3")
public:
    Q_INVOKABLE CSummaryReport();
    bool generateReport(const QString &arguments) override;
    QString visibleReportName() override { return QObject::tr("(Experimental) Summary report"); }
};

Q_DECLARE_METATYPE(CSummaryReport *);

typedef CSummaryReport *pSummaryReport;

#endif // CSUMMARYREPORT_H
