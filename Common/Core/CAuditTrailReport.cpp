/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CAuditTrailReport.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build MMS Audit Trail logs report.
*
****************************************************************************/

#include "CAuditTrailReport.h"
#include "DBStrings.h"

CAuditTrailReport::CAuditTrailReport(QObject *parent)
    : CBasicReport(parent)
{
    clearErrorString();
}

bool
CAuditTrailReport::generateReport(const QString &arguments)
{
    bool retVal = m_db->exec(audittrail::selectData.arg(arguments));
    if (retVal) {
        // Set datetime format
        QXlsx::Format dateFormat;
        setDateTimeFormat(dateFormat);

        int row = 1;
        int colTimestamp = 1;
        int colUsername = 2;
        int colRole = 3;
        int colCompanyname = 4;
        int colMethod = 5;
        int colStatus = 6;
        int colAttributes = 7;
        int colInternalIP = 8;

        QXlsx::Document xlsxReport;
        // Add header
        QVariant writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
        xlsxReport.write(row, colTimestamp, writeValue);
        writeValue = QStringLiteral("Ім'я користувача");
        xlsxReport.write(row, colUsername, writeValue);
        writeValue = QStringLiteral("Роль");
        xlsxReport.write(row, colRole, writeValue);
        writeValue = QStringLiteral("Компанія");
        xlsxReport.write(row, colCompanyname, writeValue);
        writeValue = QStringLiteral("Метод");
        xlsxReport.write(row, colMethod, writeValue);
        writeValue = QStringLiteral("Статус");
        xlsxReport.write(row, colStatus, writeValue);
        writeValue = QStringLiteral("Атрибути");
        xlsxReport.write(row, colAttributes, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        xlsxReport.write(row, colInternalIP, writeValue);
        ++row;

        while (m_db->isNext()) {
            writeValue = m_db->geValue(0).toDateTime();
            xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

            setReportDataItem(&xlsxReport, 1, colUsername, row);
            setReportDataItem(&xlsxReport, 2, colRole, row);
            setReportDataItem(&xlsxReport, 3, colCompanyname, row);
            setReportDataItem(&xlsxReport, 4, colMethod, row);
            setReportDataItem(&xlsxReport, 5, colStatus, row);
            setReportDataItem(&xlsxReport, 6, colAttributes, row);
            setReportDataItem(&xlsxReport, 7, colInternalIP, row);

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
CAuditTrailReport::setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row)
{
    CBasicReport::setReportDataItem(report, m_db, dbFieldIndex, reportFieldIndex, row);
}
