/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CBasicReport.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the reports
*
****************************************************************************/

#ifndef CBASICREPORT_H
#define CBASICREPORT_H

#include <QObject>
#include "CBasicDatabase.h"

class CBasicReport : public QObject
{
public:
    explicit CBasicReport(QObject *parent = nullptr);
    virtual ~CBasicReport() = default;
    void init(pBasicDatabase db, const QString &reportName);
    virtual bool generateReport(const QString &arguments) = 0;
    QString errorString() const { return m_errorString; }

    virtual QString visibleReportName() = 0;
    quint16 reportID() const;

protected:
    void setDateTimeFormat(QXlsx::Format &dateFormat);
    QString checkDetails(const QString &data);

    QString         m_errorString;
    QString         m_reportFileName;
    pBasicDatabase  m_db = nullptr;
};

typedef CBasicReport *pBasicReport;

#endif // CBASICREPORT_H
