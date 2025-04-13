/****************************************************************************
*
* The class for generating the consolidated MMS log reports.
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

#include "CSummaryReport.h"
#include "DBStrings.h"

namespace summaryReport {
    constexpr int colRowNumber = 1;
    constexpr int colTimestamp = 2;
    constexpr int colUsername = 3;
    constexpr int colCompanyRole = 4;
    constexpr int colTypeOrMethod = 5;
    constexpr int colStatus = 6;
    constexpr int colDetailsOrAttributes = 7;
    constexpr int colAuthType = 8;
    constexpr int colExternalIP = 9;
    constexpr int colInternalIP = 10;
    constexpr int colRequestid = 11;
}

CSummaryReport::CSummaryReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CSummaryReport::generateReport()
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
    saveReport(retVal, xlsxReport, row);
    return retVal;
}

void
CSummaryReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    const QStringList headers = {
        QStringLiteral("№"),
        QStringLiteral("Відмітка часу (за Київським часом)"),
        QStringLiteral("Ім'я користувача"),
        QStringLiteral("Роль в компанії"),
        QStringLiteral("Тип / Метод"),
        QStringLiteral("Статус / Рівень"),
        QStringLiteral("Деталі / Атрибути"),
        QStringLiteral("Тип авторизації"),
        QStringLiteral("Зовнішній IP"),
        QStringLiteral("Внутрішній IP"),
        QStringLiteral("ID запиту")
    };

    for (int col = summaryReport::colRowNumber; col <= summaryReport::colRequestid; ++col) {
        setReportDataItem(xlsxReport, col, row, headers[col - 1]);
    }
}

void
CSummaryReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat)
{
    setReportDataItem(xlsxReport, summaryReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

    QVariant writeValue = m_db->geValue("timestamp").toDateTime();
    if (!xlsxReport->write(row, summaryReport::colTimestamp, writeValue, dateFormat)) {
        throw XlsxError();
    }

    const QMap<int, QString> colMapping = {
        {summaryReport::colUsername, "username"},
        {summaryReport::colCompanyRole, "company_role"},
        {summaryReport::colTypeOrMethod, "type_or_method"},
        {summaryReport::colStatus, "status"},
        {summaryReport::colDetailsOrAttributes, "details_or_attributes"},
        {summaryReport::colAuthType, "authtype"},
        {summaryReport::colExternalIP, "externalip"},
        {summaryReport::colInternalIP, "internalip"},
        {summaryReport::colRequestid, "requestid"}
    };

    for (auto it = colMapping.begin(); it != colMapping.end(); ++it) {
        setReportDataItem(xlsxReport, it.value(), it.key(), row);
    }
}

QString
CSummaryReport::selectString() const
{
    return other::selectEvtLogAndAuditTrailData;
}
