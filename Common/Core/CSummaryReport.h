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
    Q_INVOKABLE explicit CSummaryReport(QObject *parent = nullptr);
    bool generateReport(const QString &arguments) override;
    QString visibleReportName() override { return QObject::tr("(Experimental) Summary report"); }

private:
    inline void setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row);
    inline void setReportDataItem(QXlsx::Document *report, const QString &dbFieldName, const int reportFieldIndex, const int row);
};

Q_DECLARE_METATYPE(CSummaryReport *);

typedef CSummaryReport *pSummaryReport;

#endif // CSUMMARYREPORT_H
