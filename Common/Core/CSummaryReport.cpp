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
    m_errorString.clear();
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

            writeValue = m_db->geValue(1).toString();
            xlsxReport.write(row, colUsername, writeValue);

            writeValue = m_db->geValue(2).toString();
            xlsxReport.write(row, colCompanyRole, writeValue);

            writeValue = m_db->geValue(3).toString();
            xlsxReport.write(row, colTypeOrMethod, writeValue);

            writeValue = m_db->geValue(4).toString();
            xlsxReport.write(row, colStatus, writeValue);

            writeValue = checkDetails(m_db->geValue(5).toString());
            xlsxReport.write(row, colDetailsOrAttributes, writeValue);

            writeValue = m_db->geValue(6).toString();
            xlsxReport.write(row, colAuthType, writeValue);

            writeValue = m_db->geValue(7).toString();
            xlsxReport.write(row, colExternalIP, writeValue);

            writeValue = m_db->geValue(8).toString();
            xlsxReport.write(row, colInternalIP, writeValue);

            writeValue = m_db->geValue(9).toString();
            xlsxReport.write(row, colRequestid, writeValue);

            ++row;
        } // while

        retVal = xlsxReport.saveAs(m_reportFileName);
        if (!retVal) {
            m_errorString = QStringLiteral("Error save report file");
        }
    } else {
        m_errorString = m_db->errorString();
    }

    return retVal;
}
