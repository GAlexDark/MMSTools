/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CEventLogReport.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build MMS Event logs report.
*
****************************************************************************/

#ifndef CEVENTLOGREPORT_H
#define CEVENTLOGREPORT_H

#include "CBasicReport.h"

class CEventLogReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "1")
    Q_CLASSINFO("source", "eventlog")
public:
    Q_INVOKABLE explicit CEventLogReport(QObject *parent = nullptr);
    bool generateReport() override;
    QString visibleReportName() override { return QObject::tr("Event Log"); }
    QString selectString() const override;
};

Q_DECLARE_METATYPE(CEventLogReport *);

using pEventLogReport = CEventLogReport *;

#endif // CEVENTLOGREPORT_H
