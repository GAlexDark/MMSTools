/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSummaryReport.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build MMS Event Logs + Audit Trail logs report.
*
****************************************************************************/

#include "CSummaryReport.h"
#include "DBStrings.h"

CSummaryReport::CSummaryReport(QObject *parent)
    : CBasicReport(parent)
{
    clearErrorString();
}

bool
CSummaryReport::generateReport(const QString &arguments)
{
    bool retVal = m_db->exec(other::selectEvtLogAndAuditTrailData.arg(arguments));
    if (retVal) {
        // Set datetime format
        QXlsx::Format dateFormat;
        setDateTimeFormat(dateFormat);

        int row = 1;
        int colTimestamp = 1, colUsername = 2, colCompanyRole = 3,
            colTypeOrMethod = 4, colStatus = 5, colDetailsOrAttributes = 6,
            colAuthType = 7, colExternalIP = 8, colInternalIP = 9, colRequestid = 10;

        QXlsx::Document xlsxReport;
        // Add header
        QVariant writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
        xlsxReport.write(row, colTimestamp, writeValue);
        writeValue = QStringLiteral("Ім'я користувача");
        xlsxReport.write(row, colUsername, writeValue);
        writeValue = QStringLiteral("Роль в компанії");
        xlsxReport.write(row, colCompanyRole, writeValue);
        writeValue = QStringLiteral("Тип / Метод");
        xlsxReport.write(row, colTypeOrMethod, writeValue);
        writeValue = QStringLiteral("Статус");
        xlsxReport.write(row, colStatus, writeValue);
        writeValue = QStringLiteral("Деталі / Атрибути");
        xlsxReport.write(row, colDetailsOrAttributes, writeValue);
        writeValue = QStringLiteral("Тип авторизації");
        xlsxReport.write(row, colAuthType, writeValue);
        writeValue = QStringLiteral("Зовнішній IP");
        xlsxReport.write(row, colExternalIP, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        xlsxReport.write(row, colInternalIP, writeValue);
        writeValue = QStringLiteral("ID запиту");
        xlsxReport.write(row, colRequestid, writeValue);
        ++row;

        while (m_db->isNext()) {
            writeValue = m_db->geValue(0).toDateTime();
            xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

            setReportDataItem(&xlsxReport, 1, colUsername, row);
            setReportDataItem(&xlsxReport, 2, colCompanyRole, row);
            setReportDataItem(&xlsxReport, 3, colTypeOrMethod, row);
            setReportDataItem(&xlsxReport, 4, colStatus, row);
            setReportDataItem(&xlsxReport, 5, colDetailsOrAttributes, row);
            setReportDataItem(&xlsxReport, 6, colAuthType, row);
            setReportDataItem(&xlsxReport, 7, colExternalIP, row);
            setReportDataItem(&xlsxReport, 8, colInternalIP, row);
            setReportDataItem(&xlsxReport, 9, colRequestid, row);

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

void
CSummaryReport::setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row)
{
    CBasicReport::setReportDataItem(report, m_db, dbFieldIndex, reportFieldIndex, row);
}
