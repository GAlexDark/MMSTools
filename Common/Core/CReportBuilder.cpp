#include "CReportBuilder.h"
#include "Debug.h"

const QString getAllRecords = QStringLiteral("select timestampISO8601, timestamp, externalip, username, type, details, \
                                    authtype, internalip, requestid from eventlog order by timestamp DESC;");


bool
CReportBuilder::init(const QString &dbFileName, const QString reportName)
{

    __DEBUG( Q_FUNC_INFO )

    m_reportFileName = reportName;
    bool retVal = m_db.init("QSQLITE", dbFileName);
    if (retVal) {
        retVal = m_db.open();
    }

    if (!retVal) {
        m_errorString = m_db.errorString();
        __DEBUG( m_errorString )
        m_db.close();
    }

    return retVal;
}

bool
CReportBuilder::generateReport()
{
    __DEBUG( Q_FUNC_INFO )

    // [1]  Writing excel file(*.xlsx)
    int rowHeader = 1, row = 2;
    int colTimestampISO8601 = 1, colTimestamp = 2, colExternalIP = 3, colUsername = 4, colType = 5, colDetails = 6,
            colAuthType = 7, colInternalIP = 8, colRequestid = 9;
    QString buf;

    QXlsx::Document xlsxReport;
    // Add header
    QVariant writeValue = QString("Відмітка часу (часовий пояс - UTC)");
    xlsxReport.write(rowHeader, colTimestampISO8601, writeValue);
    writeValue = QString("Відмітка часу (за Київським часом)");
    xlsxReport.write(rowHeader, colTimestamp, writeValue);
    writeValue = QString("Зовнішній IP");
    xlsxReport.write(rowHeader, colExternalIP, writeValue);
    writeValue = QString("Ім'я користувача");
    xlsxReport.write(rowHeader, colUsername, writeValue);
    writeValue = QString("Тип");
    xlsxReport.write(rowHeader, colType, writeValue);
    writeValue = QString("Деталі");
    xlsxReport.write(rowHeader, colDetails, writeValue);
    writeValue = QString("Тип авторизації");
    xlsxReport.write(rowHeader, colAuthType, writeValue);
    writeValue = QString("Внутрішній IP");
    xlsxReport.write(rowHeader, colInternalIP, writeValue);
    writeValue = QString("ID запиту");
    xlsxReport.write(rowHeader, colRequestid, writeValue);

    // Set datetime format
    QXlsx::Format dateFormat;
    dateFormat.setHorizontalAlignment(QXlsx::Format::AlignRight);
    dateFormat.setNumberFormat("dd.mm.yyyy hh:mm:ss");

    __DEBUG( xlsxReport.setColumnHidden(colTimestampISO8601, true) )

    bool retVal = m_db.exec(getAllRecords);
    if (retVal) {
        while (m_db.isNext()) {
            writeValue = m_db.geValue(0).toString();
            xlsxReport.write(row, colTimestampISO8601, writeValue);

            writeValue = m_db.geValue(1).toDateTime();
            xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

            writeValue = m_db.geValue(2).toString();
            xlsxReport.write(row, colExternalIP, writeValue);

            writeValue = m_db.geValue(3).toString();
            xlsxReport.write(row, colUsername, writeValue);

            writeValue = m_db.geValue(4).toString();
            xlsxReport.write(row, colType, writeValue);

            buf = m_db.geValue(5).toString();
            buf.replace("@N@", "\n", Qt::CaseInsensitive);
            writeValue = buf;
            xlsxReport.write(row, colDetails, writeValue);

            writeValue = m_db.geValue(6).toString();
            xlsxReport.write(row, colAuthType, writeValue);

            writeValue = m_db.geValue(7).toString();
            xlsxReport.write(row, colInternalIP, writeValue);

            writeValue = m_db.geValue(8).toString();
            xlsxReport.write(row, colRequestid, writeValue);

            ++row;
        } // while
    }

    if (retVal) {
        retVal = xlsxReport.saveAs(m_reportFileName);
        if (!retVal) {
            m_errorString = "Error save report file";
            __DEBUG( m_errorString )
        }
    } else {
        m_errorString = m_db.errorString();
        __DEBUG( m_errorString )
    }

    m_db.close();

    __DEBUG ( "CReportBuilder::generateReport DONE" )
    return retVal;
}

//----------------------------------------------------------

bool CSVThreadReportBuilder::init(const QString &dbFileName, const QString reportName)
{
    __DEBUG( Q_FUNC_INFO )

    return m_builser.init(dbFileName, reportName);;
}

void
CSVThreadReportBuilder::run()
{
    __DEBUG( Q_FUNC_INFO )

    bool retVal = m_builser.generateReport();
    if (!retVal) {
        m_errorString = m_builser.errorString();
    }
}

