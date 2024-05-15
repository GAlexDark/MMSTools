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

CEventLogReport::CEventLogReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CEventLogReport::generateReport(const QString &arguments)
{
    bool retVal = m_db->exec(eventlog::selectData.arg(arguments));
    if (retVal) {
        // Set datetime format
        QXlsx::Format dateFormat;
        setDateTimeFormat(dateFormat);

        int row = 1;
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

        QXlsx::Document xlsxReport;
        // Add header
        QVariant writeValue = QStringLiteral("№");
        xlsxReport.write(row, colRowNumber, writeValue);

        writeValue = QStringLiteral("Відмітка часу (часовий пояс - UTC)");
        xlsxReport.write(row, colTimestampISO8601, writeValue);
        writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
        xlsxReport.write(row, colTimestamp, writeValue);
        writeValue = QStringLiteral("Зовнішній IP");
        xlsxReport.write(row, colExternalIP, writeValue);
        writeValue = QStringLiteral("Ім'я користувача");
        xlsxReport.write(row, colUsername, writeValue);
        writeValue = QStringLiteral("Тип");
        xlsxReport.write(row, colType, writeValue);
        writeValue = QStringLiteral("Деталі");
        xlsxReport.write(row, colDetails, writeValue);
        writeValue = QStringLiteral("Тип авторизації");
        xlsxReport.write(row, colAuthType, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        xlsxReport.write(row, colInternalIP, writeValue);
        writeValue = QStringLiteral("ID запиту");
        xlsxReport.write(row, colRequestid, writeValue);
        ++row;

        while (m_db->isNext()) {
            xlsxReport.write(row, colRowNumber, row - 1);

            setReportDataItem(&xlsxReport, "timestampISO8601", colTimestampISO8601, row);

            writeValue = m_db->geValue("timestamp").toDateTime();
            xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

            setReportDataItem(&xlsxReport, "externalip", colExternalIP, row);
            setReportDataItem(&xlsxReport, "username", colUsername, row);
            setReportDataItem(&xlsxReport, "type", colType, row);
            setReportDataItem(&xlsxReport, "details", colDetails, row);
            setReportDataItem(&xlsxReport, "authtype", colAuthType, row);
            setReportDataItem(&xlsxReport, "internalip", colInternalIP, row);
            setReportDataItem(&xlsxReport, "requestid", colRequestid, row);

            ++row;
        } // while

        retVal = xlsxReport.saveAs(m_reportFileName);
        if (!retVal) {
            setErrorString(QStringLiteral("Error save report file"));
        }
    } else {
        setErrorString(m_db->errorString());
    }

    return retVal;
}
