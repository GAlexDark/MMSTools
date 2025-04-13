/****************************************************************************
*
* The class for generating the MMS System log reports.
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

#include "CSystemLogReport.h"
#include "DBStrings.h"

namespace systemLogReport {
    constexpr int colRowNumber = 1;
    constexpr int colTimestamp = 2;
    constexpr int colUsername = 3;
    constexpr int colRole = 4;
    constexpr int colCompanyname = 5;
    constexpr int colType = 6;
    constexpr int colSeverity = 7;
    constexpr int colMessage = 8;
}

CSystemLogReport::CSystemLogReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CSystemLogReport::generateReport()
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
CSystemLogReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    const QStringList headers = {
        QStringLiteral("№"),
        QStringLiteral("Відмітка часу (за Київським часом)"),
        QStringLiteral("Ім'я користувача"),
        QStringLiteral("Роль"),
        QStringLiteral("Компанія"),
        QStringLiteral("Тип"),
        QStringLiteral("Рівень"),
        QStringLiteral("Повідомлення")
    };

    for (int col = systemLogReport::colRowNumber; col <= systemLogReport::colMessage; ++col) {
        setReportDataItem(xlsxReport, col, row, headers[col - 1]);
    }
}

void
CSystemLogReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat)
{
    setReportDataItem(xlsxReport, systemLogReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

    QVariant writeValue = m_db->geValue("timestamp").toDateTime();
    if (!xlsxReport->write(row, systemLogReport::colTimestamp, writeValue, dateFormat)) {
        throw XlsxError();
    }

    const QMap<int, QString> colMapping = {
        {systemLogReport::colUsername, "username1"},
        {systemLogReport::colRole, "role"},
        {systemLogReport::colCompanyname, "companyname"},
        {systemLogReport::colType, "type"},
        {systemLogReport::colSeverity, "severity"},
        {systemLogReport::colMessage, "message"}
    };

    for (auto it = colMapping.begin(); it != colMapping.end(); ++it) {
        setReportDataItem(xlsxReport, it.value(), it.key(), row);
    }
}

QString
CSystemLogReport::selectString() const
{
    return systemlog::selectData;
}
