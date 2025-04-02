/****************************************************************************
*
* The class for generating the MMS Trail log reports.
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

#include "CAuditTrailReport.h"
#include "DBStrings.h"

namespace auditTrailReport {
    constexpr int colRowNumber = 1;
    constexpr int colTimestamp = 2;
    constexpr int colUsername = 3;
    constexpr int colRole = 4;
    constexpr int colCompanyname = 5;
    constexpr int colMethod = 6;
    constexpr int colStatus = 7;
    constexpr int colAttributes = 8;
    constexpr int colInternalIP = 9;
}

CAuditTrailReport::CAuditTrailReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CAuditTrailReport::generateReport()
{
    bool retVal = true;
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    QScopedPointer<QXlsx::Document> xlsxReport(new QXlsx::Document);
    int row = 1;

    try {
        addReportHeader(xlsxReport.data(), row);
        ++row;

        int multipartRowCount = getMultipartRowCount() - 1;
        while (m_db->isNext()) {
            setReportDataItem(xlsxReport.data(), auditTrailReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));
            ++row;
            if (row > maxRowsCount) {
                break;
            }
        } // while
    } catch (XlsxError &ex) {
        setErrorString(ex.what());
        retVal = false;
    }

    if (row != 2) {
        if (retVal) {
            const QString fileName = createReportFilename(row);
            retVal = xlsxReport->saveAs(fileName);
            if (!retVal) {
                setErrorString(QStringLiteral("Error save report file"));
            }
        }
    }  else {
        retVal = false;
        setErrorString(QStringLiteral("Nothing data to save"));
    }

    return retVal;
}

void
CAuditTrailReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    QVariant writeValue = QStringLiteral("№");
    setReportDataItem(xlsxReport, auditTrailReport::colRowNumber, row, writeValue);

    writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
    setReportDataItem(xlsxReport, auditTrailReport::colTimestamp, row, writeValue);
    writeValue = QStringLiteral("Ім'я користувача");
    setReportDataItem(xlsxReport, auditTrailReport::colUsername, row, writeValue);
    writeValue = QStringLiteral("Роль");
    setReportDataItem(xlsxReport, auditTrailReport::colRole, row, writeValue);
    writeValue = QStringLiteral("Компанія");
    setReportDataItem(xlsxReport, auditTrailReport::colCompanyname, row, writeValue);
    writeValue = QStringLiteral("Метод");
    setReportDataItem(xlsxReport, auditTrailReport::colMethod, row, writeValue);
    writeValue = QStringLiteral("Статус");
    setReportDataItem(xlsxReport, auditTrailReport::colStatus, row, writeValue);
    writeValue = QStringLiteral("Атрибути");
    setReportDataItem(xlsxReport, auditTrailReport::colAttributes, row, writeValue);
    writeValue = QStringLiteral("Внутрішній IP");
    setReportDataItem(xlsxReport, auditTrailReport::colInternalIP, row, writeValue);
}

void
CAuditTrailReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat)
{
    setReportDataItem(xlsxReport, auditTrailReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

    QVariant writeValue = m_db->geValue("timestamp").toDateTime();
    if (!xlsxReport->write(row, auditTrailReport::colTimestamp, writeValue, dateFormat)) {
        throw XlsxError();
    }
    setReportDataItem(xlsxReport, "username", auditTrailReport::colUsername, row);
    setReportDataItem(xlsxReport, "role", auditTrailReport::colRole, row);
    setReportDataItem(xlsxReport, "companyname", auditTrailReport::colCompanyname, row);
    setReportDataItem(xlsxReport, "method", auditTrailReport::colMethod, row);
    setReportDataItem(xlsxReport, "status", auditTrailReport::colStatus, row);
    setReportDataItem(xlsxReport, "attributes", auditTrailReport::colAttributes, row);
    setReportDataItem(xlsxReport, "internalip", auditTrailReport::colInternalIP, row);
}

QString
CAuditTrailReport::selectString() const
{
    return audittrail::selectData;
}
