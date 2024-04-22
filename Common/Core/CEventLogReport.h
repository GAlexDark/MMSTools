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
public:
    Q_INVOKABLE CEventLogReport(QObject *parent = nullptr);
    bool generateReport(const QString &arguments) override;
    QString visibleReportName() override { return QObject::tr("Event Log"); }
};

Q_DECLARE_METATYPE(CEventLogReport *);

typedef CEventLogReport *pEventLogReport;

#endif // CEVENTLOGREPORT_H
