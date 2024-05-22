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
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CAuditTrailReport::generateReport()
{
    bool retVal = true;
    // Set datetime format
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    int colRowNumber = 1;
    int colTimestamp = 2;
    int colUsername = 3;
    int colRole = 4;
    int colCompanyname = 5;
    int colMethod = 6;
    int colStatus = 7;
    int colAttributes = 8;
    int colInternalIP = 9;

    QScopedPointer<QXlsx::Document> xlsxReport(new QXlsx::Document);
    int row = 1;
    // Add header
    QVariant writeValue;
    try {
        writeValue = QStringLiteral("№");
        setReportDataItem(xlsxReport.data(), colRowNumber, row, writeValue);

        writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
        setReportDataItem(xlsxReport.data(), colTimestamp, row, writeValue);
        writeValue = QStringLiteral("Ім'я користувача");
        setReportDataItem(xlsxReport.data(), colUsername, row, writeValue);
        writeValue = QStringLiteral("Роль");
        setReportDataItem(xlsxReport.data(), colRole, row, writeValue);
        writeValue = QStringLiteral("Компанія");
        setReportDataItem(xlsxReport.data(), colCompanyname, row, writeValue);
        writeValue = QStringLiteral("Метод");
        setReportDataItem(xlsxReport.data(), colMethod, row, writeValue);
        writeValue = QStringLiteral("Статус");
        setReportDataItem(xlsxReport.data(), colStatus, row, writeValue);
        writeValue = QStringLiteral("Атрибути");
        setReportDataItem(xlsxReport.data(), colAttributes, row, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        setReportDataItem(xlsxReport.data(), colInternalIP, row, writeValue);
        ++row;
        int multipartRowCount = getMultipartRowCount() - 1;
        while (m_db->isNext()) {
            setReportDataItem(xlsxReport.data(), colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

            writeValue = m_db->geValue("timestamp").toDateTime();
            if (!xlsxReport->write(row, colTimestamp, writeValue, dateFormat)) {
                throw "Write error";
            }
            setReportDataItem(xlsxReport.data(), "username", colUsername, row);
            setReportDataItem(xlsxReport.data(), "role", colRole, row);
            setReportDataItem(xlsxReport.data(), "companyname", colCompanyname, row);
            setReportDataItem(xlsxReport.data(), "method", colMethod, row);
            setReportDataItem(xlsxReport.data(), "status", colStatus, row);
            setReportDataItem(xlsxReport.data(), "attributes", colAttributes, row);
            setReportDataItem(xlsxReport.data(), "internalip", colInternalIP, row);
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
CAuditTrailReport::selectString() const
{
    return audittrail::selectData;
}
