/****************************************************************************
*
* The class for generating the MMS Event log reports.
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

#include "CEventLogReport.h"
#include <QScopedPointer>
#include "DBStrings.h"

namespace eventLogReport {
    constexpr int colRowNumber = 1;
    constexpr int colTimestampISO8601 = 2;
    constexpr int colTimestamp = 3;
    constexpr int colExternalIP = 4;
    constexpr int colUsername = 5;
    constexpr int colType = 6;
    constexpr int colDetails = 7;
    constexpr int colAuthType = 8;
    constexpr int colInternalIP = 9;
    constexpr int colRequestid = 10;
}

CEventLogReport::CEventLogReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CEventLogReport::generateReport()
{
    bool retVal = true;
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    QScopedPointer<QXlsx::Document> xlsxReport(new QXlsx::Document);
    int row = 1;

    try {
        addReportHeader(xlsxReport.data(), row);
        ++row;

        bool isDataTooLong;
        int multipartRowCount = getMultipartRowCount() - 1;

        while (m_db->isNext()) {
            addReportRow(xlsxReport.data(), row, multipartRowCount, dateFormat, isDataTooLong);
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
CEventLogReport::addReportHeader(QXlsx::Document *xlsxReport, int row)
{
    QVariant writeValue = QStringLiteral("№");
    setReportDataItem(xlsxReport, eventLogReport::colRowNumber, row, writeValue);

    writeValue = QStringLiteral("Відмітка часу (часовий пояс - UTC)");
    setReportDataItem(xlsxReport, eventLogReport::colTimestampISO8601, row, writeValue);
    writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
    setReportDataItem(xlsxReport, eventLogReport::colTimestamp, row, writeValue);
    writeValue = QStringLiteral("Зовнішній IP");
    setReportDataItem(xlsxReport, eventLogReport::colExternalIP, row, writeValue);
    writeValue = QStringLiteral("Ім'я користувача");
    setReportDataItem(xlsxReport, eventLogReport::colUsername, row, writeValue);
    writeValue = QStringLiteral("Тип");
    setReportDataItem(xlsxReport, eventLogReport::colType, row, writeValue);
    writeValue = QStringLiteral("Деталі");
    setReportDataItem(xlsxReport, eventLogReport::colDetails, row, writeValue);
    writeValue = QStringLiteral("Тип авторизації");
    setReportDataItem(xlsxReport, eventLogReport::colAuthType, row, writeValue);
    writeValue = QStringLiteral("Внутрішній IP");
    setReportDataItem(xlsxReport, eventLogReport::colInternalIP, row, writeValue);
    writeValue = QStringLiteral("ID запиту");
    setReportDataItem(xlsxReport, eventLogReport::colRequestid, row, writeValue);
}

void
CEventLogReport::addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat, bool &isDataTooLong)
{
    setReportDataItem(xlsxReport, eventLogReport::colRowNumber, row, QVariant::fromValue(multipartRowCount + row));
    setReportDataItem(xlsxReport, "timestampISO8601", eventLogReport::colTimestampISO8601, row);

    QDateTime timeStamp = m_db->geValue("timestamp").toDateTime();
    if (!isShowMilliseconds()) {
        QTime time = timeStamp.time();
        time.setHMS(time.hour(), time.minute(), time.second(), 0);
        timeStamp.setTime(time);
    }
    if (!xlsxReport->write(row, eventLogReport::colTimestamp, timeStamp, dateFormat)) {
        throw XlsxError();
    }
    setReportDataItem(xlsxReport, "externalip", eventLogReport::colExternalIP, row);
    setReportDataItem(xlsxReport, "username", eventLogReport::colUsername, row);
    setReportDataItem(xlsxReport, "type", eventLogReport::colType, row);

    QVariant writeValue = checkDetails(m_db->geValue("details").toString(), isDataTooLong);
    if (!xlsxReport->write(row, eventLogReport::colDetails, writeValue) && !isDataTooLong) {
        throw XlsxError();
    }
    setReportDataItem(xlsxReport, "authtype", eventLogReport::colAuthType, row);
    setReportDataItem(xlsxReport, "internalip", eventLogReport::colInternalIP, row);
    setReportDataItem(xlsxReport, "requestid", eventLogReport::colRequestid, row);
}

QString
CEventLogReport::selectString() const
{
    return eventlog::selectData;
}
