/****************************************************************************
*
* The Base class for the MMS log reports.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CBASICREPORT_H
#define CBASICREPORT_H

#include <QObject>
#include <QStringList>
#include <QString>

#include "MMSTypes.h"
#include "CSqliteDatabase.h"

const int maxRowsCount = 1048575;

class CBasicReport : public QObject
{
public:
    explicit CBasicReport(QObject *parent = nullptr);
    void init(pSqliteDatabase db, const QString &reportName, bool showMilliseconds);
    bool generateReport(const QString &request);
    virtual bool generateReport() = 0;
    const QString& errorString() const { return m_errorString; }
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
    bool isShowMilliseconds() const { return m_showMilliseconds; }

    pSqliteDatabase m_db = nullptr;

private:
    QString createReportPartFilename();

    QString m_errorString;
    QString m_reportFileName;
    int m_reportPartNumber = 1;
    int m_multipartRowCount = 0;
    bool m_isMultipartReport = false;
    bool m_showMilliseconds = false;
};

using pBasicReport = CBasicReport *;

class XlsxError : public mms::MmsCommonException
{
public:
    explicit XlsxError(const QString &text = "QXlsx write error") noexcept
        : MmsCommonException(text) {}
};
#endif // CBASICREPORT_H
