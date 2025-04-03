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
            addReportRow(xlsxReport.data(), row, multipartRowCount, dateFormat);
            ++row;
            if (row > maxRowsCount) {
                break;
            }
        }
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
    } else {
        retVal = false;
        setErrorString(QStringLiteral("Nothing data to save"));
    }

    return retVal;
}

void
CAuditTrailReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    const QStringList headers = {
        QStringLiteral("№"),
        QStringLiteral("Відмітка часу (за Київським часом)"),
        QStringLiteral("Ім'я користувача"),
        QStringLiteral("Роль"),
        QStringLiteral("Компанія"),
        QStringLiteral("Метод"),
        QStringLiteral("Статус"),
        QStringLiteral("Атрибути"),
        QStringLiteral("Внутрішній IP")
    };

    for (int col = auditTrailReport::colRowNumber; col <= auditTrailReport::colInternalIP; ++col) {
        setReportDataItem(xlsxReport, col, row, headers[col - 1]);
    }
}

void
CAuditTrailReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat)
{
    setReportDataItem(xlsxReport, auditTrailReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

    QVariant writeValue = m_db->geValue("timestamp").toDateTime();
    if (!xlsxReport->write(row, auditTrailReport::colTimestamp, writeValue, dateFormat)) {
        throw XlsxError();
    }

    const QMap<int, QString> colMapping = {
        {auditTrailReport::colUsername, "username"},
        {auditTrailReport::colRole, "role"},
        {auditTrailReport::colCompanyname, "companyname"},
        {auditTrailReport::colMethod, "method"},
        {auditTrailReport::colStatus, "status"},
        {auditTrailReport::colAttributes, "attributes"},
        {auditTrailReport::colInternalIP, "internalip"}
    };

    for (auto it = colMapping.begin(); it != colMapping.end(); ++it) {
        setReportDataItem(xlsxReport, it.value(), it.key(), row);
    }
}

QString
CAuditTrailReport::selectString() const
{
    return audittrail::selectData;
}
