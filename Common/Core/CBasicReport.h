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

const int maxRowsCount = 1048575;

class CBasicReport : public QObject
{
public:
    explicit CBasicReport(QObject *parent = nullptr);
    void init(pBasicDatabase db, const QString &reportName, bool showMilliseconds);
    bool generateReport(const QString &request);
    virtual bool generateReport() = 0;
    QString errorString() const { return m_errorString; }
    virtual QString selectString() const = 0;
    virtual QString visibleReportName() = 0;
    quint16 reportID() const;
    QStringList sources() const;

protected:
    void setDateTimeFormat(QXlsx::Format &dateFormat);
    QString checkDetails(const QString &data, bool &isDataTooLong);
    void setErrorString(const QString &errorString) { m_errorString = errorString; }
    void clearErrorString() { m_errorString.clear(); }
    void setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row);
    void setReportDataItem(QXlsx::Document *report, const QString &dbFieldName, const int reportFieldIndex, const int row);
    void setReportDataItem(QXlsx::Document *report, const int column, const int row, const QVariant &writeValue);
    QString createReportFilename(const int row);
    int getMultipartRowCount() const { return m_multipartRowCount; }

    pBasicDatabase m_db = nullptr;

private:
    QString createReportPartFilename();

    QString m_errorString;
    QString m_reportFileName;
    int m_reportPartNumber = 1;
    int m_multipartRowCount = 0;
    bool m_isMultipartReport = false;
    bool m_showMilliseconds = false;
};

typedef CBasicReport *pBasicReport;

#endif // CBASICREPORT_H
