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
    m_errorString.clear();
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
        int colTimestamp = 1,
            colUsername = 2, colRole = 3, colCompanyname = 4,
            colMethod = 5, colStatus = 6, colAttributes = 7, colInternalIP = 8;

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

            writeValue = m_db->geValue(1).toString();
            xlsxReport.write(row, colUsername, writeValue);

            writeValue = m_db->geValue(2).toString();
            xlsxReport.write(row, colRole, writeValue);

            writeValue = m_db->geValue(3).toString();
            xlsxReport.write(row, colCompanyname, writeValue);

            writeValue = m_db->geValue(4).toString();
            xlsxReport.write(row, colMethod, writeValue);

            writeValue = m_db->geValue(5).toString();
            xlsxReport.write(row, colStatus, writeValue);

            writeValue = m_db->geValue(6).toString();
            xlsxReport.write(row, colAttributes, writeValue);

            writeValue = m_db->geValue(7).toString();
            xlsxReport.write(row, colInternalIP, writeValue);

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
