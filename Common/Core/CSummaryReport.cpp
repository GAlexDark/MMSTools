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
    } else {
        retVal = false;
        setErrorString(QStringLiteral("Nothing data to save"));
    }

    return retVal;
}

void
CSummaryReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    QVariant writeValue = QStringLiteral("№");
    setReportDataItem(xlsxReport, summaryReport::colRowNumber, row, writeValue);

    writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
    setReportDataItem(xlsxReport, summaryReport::colTimestamp, row, writeValue);
    writeValue = QStringLiteral("Ім'я користувача");
    setReportDataItem(xlsxReport, summaryReport::colUsername, row, writeValue);
    writeValue = QStringLiteral("Роль в компанії");
    setReportDataItem(xlsxReport, summaryReport::colCompanyRole, row, writeValue);
    writeValue = QStringLiteral("Тип / Метод");
    setReportDataItem(xlsxReport, summaryReport::colTypeOrMethod, row, writeValue);
    writeValue = QStringLiteral("Статус / Рівень");
    setReportDataItem(xlsxReport, summaryReport::colStatus, row, writeValue);
    writeValue = QStringLiteral("Деталі / Атрибути");
    setReportDataItem(xlsxReport, summaryReport::colDetailsOrAttributes, row, writeValue);
    writeValue = QStringLiteral("Тип авторизації");
    setReportDataItem(xlsxReport, summaryReport::colAuthType, row, writeValue);
    writeValue = QStringLiteral("Зовнішній IP");
    setReportDataItem(xlsxReport, summaryReport::colExternalIP, row, writeValue);
    writeValue = QStringLiteral("Внутрішній IP");
    setReportDataItem(xlsxReport, summaryReport::colInternalIP, row, writeValue);
    writeValue = QStringLiteral("ID запиту");
    setReportDataItem(xlsxReport, summaryReport::colRequestid, row, writeValue);
}

void
CSummaryReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat)
{
    setReportDataItem(xlsxReport, summaryReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

    QVariant writeValue = m_db->geValue("timestamp").toDateTime();
    if (!xlsxReport->write(row, summaryReport::colTimestamp, writeValue, dateFormat)) {
        throw XlsxError();
    }
    setReportDataItem(xlsxReport, "username", summaryReport::colUsername, row);
    setReportDataItem(xlsxReport, "company_role", summaryReport::colCompanyRole, row);
    setReportDataItem(xlsxReport, "type_or_method", summaryReport::colTypeOrMethod, row);
    setReportDataItem(xlsxReport, "status", summaryReport::colStatus, row);
    setReportDataItem(xlsxReport, "details_or_attributes", summaryReport::colDetailsOrAttributes, row);
    setReportDataItem(xlsxReport, "authtype", summaryReport::colAuthType, row);
    setReportDataItem(xlsxReport, "externalip", summaryReport::colExternalIP, row);
    setReportDataItem(xlsxReport, "internalip", summaryReport::colInternalIP, row);
    setReportDataItem(xlsxReport, "requestid", summaryReport::colRequestid, row);
}

QString
CSummaryReport::selectString() const
{
    return other::selectEvtLogAndAuditTrailData;
}
