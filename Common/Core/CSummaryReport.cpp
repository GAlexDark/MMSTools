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
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CSummaryReport::generateReport()
{
    bool retVal = true;
    // Set datetime format
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    int colRowNumber = 1;
    int colTimestamp = 2;
    int colUsername = 3;
    int colCompanyRole = 4;
    int colTypeOrMethod = 5;
    int colStatus = 6;
    int colDetailsOrAttributes = 7;
    int colAuthType = 8;
    int colExternalIP = 9;
    int colInternalIP = 10;
    int colRequestid = 11;

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
        writeValue = QStringLiteral("Роль в компанії");
        setReportDataItem(xlsxReport.data(), colCompanyRole, row, writeValue);
        writeValue = QStringLiteral("Тип / Метод");
        setReportDataItem(xlsxReport.data(), colTypeOrMethod, row, writeValue);
        writeValue = QStringLiteral("Статус");
        setReportDataItem(xlsxReport.data(), colStatus, row, writeValue);
        writeValue = QStringLiteral("Деталі / Атрибути");
        setReportDataItem(xlsxReport.data(), colDetailsOrAttributes, row, writeValue);
        writeValue = QStringLiteral("Тип авторизації");
        setReportDataItem(xlsxReport.data(), colAuthType, row, writeValue);
        writeValue = QStringLiteral("Зовнішній IP");
        setReportDataItem(xlsxReport.data(), colExternalIP, row, writeValue);
        writeValue = QStringLiteral("Внутрішній IP");
        setReportDataItem(xlsxReport.data(), colInternalIP, row, writeValue);
        writeValue = QStringLiteral("ID запиту");
        setReportDataItem(xlsxReport.data(), colRequestid, row, writeValue);
        ++row;
        int multipartRowCount = getMultipartRowCount() - 1;
        while (m_db->isNext()) {
            setReportDataItem(xlsxReport.data(), colRowNumber, row, QVariant::fromValue(multipartRowCount + row));

            writeValue = m_db->geValue("timestamp").toDateTime();
            if (!xlsxReport->write(row, colTimestamp, writeValue, dateFormat)) {
                throw XlsxError();
            }
            setReportDataItem(xlsxReport.data(), "username", colUsername, row);
            setReportDataItem(xlsxReport.data(), "company_role", colCompanyRole, row);
            setReportDataItem(xlsxReport.data(), "type_or_method", colTypeOrMethod, row);
            setReportDataItem(xlsxReport.data(), "status", colStatus, row);
            setReportDataItem(xlsxReport.data(), "details_or_attributes", colDetailsOrAttributes, row);
            setReportDataItem(xlsxReport.data(), "authtype", colAuthType, row);
            setReportDataItem(xlsxReport.data(), "externalip", colExternalIP, row);
            setReportDataItem(xlsxReport.data(), "internalip", colInternalIP, row);
            setReportDataItem(xlsxReport.data(), "requestid", colRequestid, row);
            ++row;

            if (row > maxRowsCount) {
                break;
            }
        } // while
    } catch (XlsxError &ex) {
        setErrorString(ex.what());
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
CSummaryReport::selectString() const
{
    return other::selectEvtLogAndAuditTrailData;
}
