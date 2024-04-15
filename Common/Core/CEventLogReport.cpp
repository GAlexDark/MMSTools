#include "CEventLogReport.h"
#include "DBStrings.h"

CEventLogReport::CEventLogReport()
{
    m_errorString.clear();
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
        int colTimestampISO8601 = 1, colTimestamp = 2, colExternalIP = 3, colUsername = 4, colType = 5, colDetails = 6,
            colAuthType = 7, colInternalIP = 8, colRequestid = 9;

        QXlsx::Document xlsxReport;
        // Add header
        QVariant writeValue = QStringLiteral("Відмітка часу (часовий пояс - UTC)");
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
            writeValue = m_db->geValue(0).toString();
            xlsxReport.write(row, colTimestampISO8601, writeValue);

            writeValue = m_db->geValue(1).toDateTime();
            xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

            writeValue = m_db->geValue(2).toString();
            xlsxReport.write(row, colExternalIP, writeValue);

            writeValue = m_db->geValue(3).toString();
            xlsxReport.write(row, colUsername, writeValue);

            writeValue = m_db->geValue(4).toString();
            xlsxReport.write(row, colType, writeValue);

            writeValue = checkDetails(m_db->geValue(5).toString());
            xlsxReport.write(row, colDetails, writeValue);

            writeValue = m_db->geValue(6).toString();
            xlsxReport.write(row, colAuthType, writeValue);

            writeValue = m_db->geValue(7).toString();
            xlsxReport.write(row, colInternalIP, writeValue);

            writeValue = m_db->geValue(8).toString();
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
