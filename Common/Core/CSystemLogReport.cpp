/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSystemLogReport.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for build the MMS System logs report.
*
****************************************************************************/

#include "CSystemLogReport.h"
#include "DBStrings.h"
#include "MMSTypes.h"

CSystemLogReport::CSystemLogReport(QObject *parent)
    : CBasicReport{parent}
{
    clearErrorString();
}

bool
CSystemLogReport::generateReport()
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
    int colSeverity = 6;
    int colMessage = 7;

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
        writeValue = QStringLiteral("Рівень");
        setReportDataItem(xlsxReport.data(), colSeverity, row, writeValue);
        writeValue = QStringLiteral("Повідомлення");
        setReportDataItem(xlsxReport.data(), colMessage, row, writeValue);

        ++row;
        int multipartRowCount = getMultipartRowCount() - 1;
        while (m_db->isNext()) {
            setReportDataItem(xlsxReport.data(), colRowNumber, row, QVariant::fromValue(multipartRowCount + row));
            throw mms::XlsxError();
            writeValue = m_db->geValue("timestamp").toDateTime();
            if (!xlsxReport->write(row, colTimestamp, writeValue, dateFormat)) {
                throw mms::XlsxError();
            }
            setReportDataItem(xlsxReport.data(), "username1", colUsername, row);
            setReportDataItem(xlsxReport.data(), "role", colRole, row);
            setReportDataItem(xlsxReport.data(), "companyname", colCompanyname, row);
            setReportDataItem(xlsxReport.data(), "severity", colSeverity, row);
            setReportDataItem(xlsxReport.data(), "message", colMessage, row);
            ++row;
            if (row > maxRowsCount) {
                break;
            }
        } // while
    } catch (mms::XlsxError &ex) {
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
CSystemLogReport::selectString() const
{
    return systemlog::selectData;
}
