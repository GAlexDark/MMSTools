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
    void init(pBasicDatabase db, const QString &reportName, bool showMilliseconds);
    virtual bool generateReport(const QString &arguments) = 0;
    QString errorString() const { return m_errorString; }

    virtual QString visibleReportName() = 0;
    quint16 reportID() const;

protected:
    void setDateTimeFormat(QXlsx::Format &dateFormat);
    QString checkDetails(const QString &data);
    void setErrorString(const QString &errorString) { m_errorString = errorString; }
    void clearErrorString() { m_errorString.clear(); }
    void setReportDataItem(QXlsx::Document *report, const pBasicDatabase db, const int dbFieldIndex, const int reportFieldIndex, const int row);

    QString         m_reportFileName;
    pBasicDatabase  m_db = nullptr;
    bool            m_showMilliseconds = false;

private:
    QString         m_errorString;
};

typedef CBasicReport *pBasicReport;

#endif // CBASICREPORT_H
