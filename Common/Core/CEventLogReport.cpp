/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CEventLogReport.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build MMS Event logs report.
*
****************************************************************************/

#include "CEventLogReport.h"
#include "DBStrings.h"
#include <QScopedPointer>

CEventLogReport::CEventLogReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CEventLogReport::generateReport()
{
    bool retVal = true;
    // Set datetime format
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    int colRowNumber = 1;
    int colTimestampISO8601 = 2;
    int colTimestamp = 3;
    int colExternalIP = 4;
    int colUsername = 5;
    int colType = 6;
    int colDetails = 7;
    int colAuthType = 8;
    int colInternalIP = 9;
    int colRequestid = 10;

    QScopedPointer<QXlsx::Document> xlsxReport(new QXlsx::Document);
    int row = 1;
    // Add header
    QVariant writeValue;
    try {
        writeValue = QStringLiteral("№");
        setReportDataItem(xlsxReport.data(), colRowNumber, row, writeValue);

        writeValue = QStringLiteral("Відмітка часу (часовий пояс - UTC)");
        setReportDataItem(xlsxReport.data(), colTimestampISO8601, row, writeValue);
        writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
        setReportDataItem(xlsxReport.data(), colTimestamp, row, writeValue);
        writeValue = QStringLiteral("Зовнішній IP");
        setReportDataItem(xlsxReport.data(), colExternalIP, row, writeValue);
        writeValue = QStringLiteral("Ім'я користувача");
        setReportDataItem(xlsxReport.data(), colUsername, row, writeValue);
        writeValue = QStringLiteral("Тип");
        setReportDataItem(xlsxReport.data(), colType, row, writeValue);
        writeValue = QStringLiteral("Деталі");
        setReportDataItem(xlsxReport.data(), colDetails, row, writeValue);
        writeValue = QStringLiteral("Тип авторизації");
        setReportDataItem(xlsxReport.data(), colAuthType, row, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        setReportDataItem(xlsxReport.data(), colInternalIP, row, writeValue);
        writeValue = QStringLiteral("ID запиту");
        setReportDataItem(xlsxReport.data(), colRequestid, row, writeValue);
        ++row;
        bool isDataTooLong;
        int multipartRowCount = getMultipartRowCount() - 1;
        while (m_db->isNext()) {
            setReportDataItem(xlsxReport.data(), colRowNumber, row, QVariant::fromValue(multipartRowCount + row));
            setReportDataItem(xlsxReport.data(), "timestampISO8601", colTimestampISO8601, row);

            writeValue = m_db->geValue("timestamp").toDateTime();
            if (!xlsxReport->write(row, colTimestamp, writeValue, dateFormat)) {
                throw "Write error";
            }
            setReportDataItem(xlsxReport.data(), "externalip", colExternalIP, row);
            setReportDataItem(xlsxReport.data(), "username", colUsername, row);
            setReportDataItem(xlsxReport.data(), "type", colType, row);

            writeValue = checkDetails(m_db->geValue("details").toString(), isDataTooLong);
            if (!xlsxReport->write(row, colDetails, writeValue) && !isDataTooLong) {
                throw "Write error";
            }
            setReportDataItem(xlsxReport.data(), "authtype", colAuthType, row);
            setReportDataItem(xlsxReport.data(), "internalip", colInternalIP, row);
            setReportDataItem(xlsxReport.data(), "requestid", colRequestid, row);
            ++row;

            if (row > maxRowsCount) {
                break;
            }
        } // while
    } catch (const char* ex) {
        setErrorString(ex);
        retVal = false;
    }
    if (retVal) {
        const QString fileName = createReportFilename(row);
        retVal = xlsxReport->saveAs(fileName);
        if (!retVal) {
            setErrorString(QStringLiteral("Error save report file"));
        }
    }
    return retVal;
}

QString
CEventLogReport::selectString() const
{
    return eventlog::selectData;
}
