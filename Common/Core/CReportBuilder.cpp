/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CReportBuilder.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The classes for creating the report from MMS Event Logs.
*
****************************************************************************/

#include "CReportBuilder.h"
//#include "Debug.h"
#include "DBStrings.h"
#include "elcUtils.h"

CReportBuilder::CReportBuilder()
    : m_db(nullptr), m_errorString(""), m_reportFileName("")
{}

CReportBuilder::~CReportBuilder()
{
    delete m_db;
    m_db = nullptr;

    m_includedUsernamesList.clear();
    m_excludedUsernamesList.clear();
}

bool
CReportBuilder::init(const QString &dbFileName, const QString &reportName, const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList)
{
    Q_CHECK_PTR(excludedUsernamesList);
    Q_CHECK_PTR(includedUsernamesList);
    bool retVal = true;
    try {
        m_db = new CBasicDatabase();

        m_reportFileName = reportName;
        m_excludedUsernamesList = *excludedUsernamesList;
        m_includedUsernamesList = *includedUsernamesList;

        retVal = m_db->init(QStringLiteral("QSQLITE"), dbFileName);
        if (retVal) {
            retVal = m_db->open();
            if (retVal) {
                int blockSize = elcUtils::getStorageBlockSize(dbFileName);
                QStringList pragmaItems;
                pragmaItems.append(pragmaUTF8);
                pragmaItems.append(pragmaPageSize.arg(blockSize));

                for (qsizetype i = 0; i < pragmaItems.size(); ++i) {
                    retVal = m_db->exec(pragmaItems.at(i));
                    if (!retVal) {
                        break;
                    }
                }
            }
        }
    } catch (const std::bad_alloc &e) {
        retVal = false;
        m_errorString = e.what();
    }

    if (!retVal) {
        m_errorString = m_db->errorString();
        m_db->close();
    }

    return retVal;
}

bool
CReportBuilder::generateReport()
{
    int rowHeader = 1;
    int colTimestampISO8601 = 1, colTimestamp = 2, colExternalIP = 3, colUsername = 4, colType = 5, colDetails = 6,
            colAuthType = 7, colInternalIP = 8, colRequestid = 9;
    QString buf;

    QXlsx::Document xlsxReport;
    // Add header
    QVariant writeValue = QStringLiteral("Відмітка часу (часовий пояс - UTC)");
    xlsxReport.write(rowHeader, colTimestampISO8601, writeValue);
    writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
    xlsxReport.write(rowHeader, colTimestamp, writeValue);
    writeValue = QStringLiteral("Зовнішній IP");
    xlsxReport.write(rowHeader, colExternalIP, writeValue);
    writeValue = QStringLiteral("Ім'я користувача");
    xlsxReport.write(rowHeader, colUsername, writeValue);
    writeValue = QStringLiteral("Тип");
    xlsxReport.write(rowHeader, colType, writeValue);
    writeValue = QStringLiteral("Деталі");
    xlsxReport.write(rowHeader, colDetails, writeValue);
    writeValue = QStringLiteral("Тип авторизації");
    xlsxReport.write(rowHeader, colAuthType, writeValue);
    writeValue = QStringLiteral("Внутрішній IP");
    xlsxReport.write(rowHeader, colInternalIP, writeValue);
    writeValue = QStringLiteral("ID запиту");
    xlsxReport.write(rowHeader, colRequestid, writeValue);

    // Set datetime format
    QXlsx::Format dateFormat;
    dateFormat.setHorizontalAlignment(QXlsx::Format::AlignRight);
    dateFormat.setNumberFormat(QStringLiteral("dd.mm.yyyy hh:mm:ss"));

    QString args;
    args.clear();
    if (m_includedUsernamesList.isEmpty()) {
        if (!m_excludedUsernamesList.isEmpty()) {
            args.append(QStringLiteral("WHERE "));
            qsizetype size = m_excludedUsernamesList.size() - 1;
            for (qsizetype i = 0; i < size; ++i) {
                args.append(QStringLiteral("e.username<>'%1' AND ").arg(m_excludedUsernamesList.at(i)));
            } //for
            args.append(QStringLiteral("e.username<>'%1'").arg(m_excludedUsernamesList.at(size)));
        }
    } else {
        args.append(QStringLiteral("WHERE "));
        qsizetype size = m_includedUsernamesList.size() - 1;
        for (qsizetype i = 0; i < size; ++i) {
            args.append(QStringLiteral("e.username='%1' OR ").arg(m_includedUsernamesList.at(i)));
        } //for
        args.append(QStringLiteral("e.username='%1'").arg(m_includedUsernamesList.at(size)));
    }
//    __DEBUG( getAllRecords.arg(args) )
    const QString eolMaskCode = QStringLiteral("@N@");
    const QString eolChar = QStringLiteral("\n");
    bool retVal = m_db->exec(getAllRecords.arg(args));
    if (retVal) {
        int row = 2;
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

            buf = m_db->geValue(5).toString();
            buf.replace(eolMaskCode, eolChar, Qt::CaseInsensitive);
            writeValue = buf;
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

    m_db->close();

    return retVal;
}

//----------------------------------------------------------

CSVThreadReportBuilder::CSVThreadReportBuilder(): m_errorString(""), m_retVal(false)
{}

bool
CSVThreadReportBuilder::init(const QString &dbFileName, const QString &reportName, const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList)
{
    Q_CHECK_PTR(excludedUsernamesList);
    Q_CHECK_PTR(includedUsernamesList);
    return m_builser.init(dbFileName, reportName, excludedUsernamesList, includedUsernamesList);
}

void
CSVThreadReportBuilder::run()
{
    m_errorString.clear();
    m_retVal = m_builser.generateReport();
    if (!m_retVal) {
        m_errorString = m_builser.errorString();
    }
}

