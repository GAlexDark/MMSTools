/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSystemLogReport.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build the MMS System logs report.
*
****************************************************************************/

#ifndef CSYSTEMLOGREPORT_H
#define CSYSTEMLOGREPORT_H

#include "CBasicReport.h"

class CSystemLogReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "3") //4
    Q_CLASSINFO("source", "systemlog")
public:
    Q_INVOKABLE explicit CSystemLogReport(QObject *parent = nullptr);
    bool generateReport() override;
    QString visibleReportName() override { return QObject::tr("System Log"); }
    QString selectString() const override;
};

Q_DECLARE_METATYPE(CSystemLogReport *);

using pSystemLogReport = CSystemLogReport *;

#endif // CSYSTEMLOGREPORT_H
