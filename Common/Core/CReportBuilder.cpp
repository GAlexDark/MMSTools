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
#include "DBStrings.h"
#include "elcUtils.h"
#include "CReportManager.h"

CReportBuilder::CReportBuilder()
{
    m_errorString.clear();
    m_excludedUsernamesList.clear();
    m_includedUsernamesList.clear();
}

CReportBuilder::~CReportBuilder()
{
    m_includedUsernamesList.clear();
    m_excludedUsernamesList.clear();
}

bool
CReportBuilder::init(quint16 logID, const QString &dbFileName, const QString &reportName,
                     const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList, const bool showMilliseconds)
{
    Q_CHECK_PTR(excludedUsernamesList);
    m_excludedUsernamesList = *excludedUsernamesList;
    Q_CHECK_PTR(includedUsernamesList);
    m_includedUsernamesList = *includedUsernamesList;

    CReportManager &reportManager = CReportManager::instance();
    bool retVal = reportManager.checkID(logID);
    if (retVal) {
        m_report = reportManager.getInstance(logID);
        Q_CHECK_PTR(m_report);
        retVal = m_db.init(QLatin1String("QSQLITE"), dbFileName);
        if (retVal) {
            retVal = m_db.open();
            if (retVal) {
                int blockSize = elcUtils::getStorageBlockSize(dbFileName);
                QStringList pragmaItems;
                pragmaItems.append(pragmaUTF8);
                pragmaItems.append(pragmaPageSize.arg(blockSize));
                for (const QString &item : pragmaItems) {
                    retVal = m_db.exec(item);
                    if (!retVal) {
                        break;
                    }
                }
            }
        }
        if (retVal) {
            QStringList tables = m_report->sources();
            if (!tables.isEmpty()) {
                QString table;
                retVal = m_db.checkTables(tables, table);
                if (retVal) {
                    m_report->init(&m_db, reportName, showMilliseconds);
                } else {
                    m_errorString = QStringLiteral("The table '%1' not found. The report cannot create results.").arg(table);
                }
            } else {
                m_errorString = QStringLiteral("List of tables not found. The report cannot check them.");
            }
        } else {
            m_errorString = m_db.errorString();
        }
        if (!retVal) {
            m_db.close();
        }
    } else {
        m_errorString = QStringLiteral("The report not found");
    }

    return retVal;
}

bool
CReportBuilder::generateReport()
{
    QString args;
    args.clear();

    bool retVal = true;
    bool isIncluded = !m_includedUsernamesList.isEmpty();
    bool isExcluded = !m_excludedUsernamesList.isEmpty();
    if (isIncluded && isExcluded) {
        m_errorString = QStringLiteral("User exclusion and inclusion lists cannot be specified at the same time.");
        retVal = false;
    } else {
        if (isIncluded || isExcluded) {
            args.append(QLatin1String("WHERE "));
            if (isIncluded) {
                qsizetype size = m_includedUsernamesList.size() - 1;
                for (qsizetype i = 0; i < size; ++i) {
                    args.append(QStringLiteral("username='%1' OR ").arg(m_includedUsernamesList.at(i)));
                } //for
                args.append(QStringLiteral("username='%1'").arg(m_includedUsernamesList.at(size)));
            }
            if (isExcluded) {
                qsizetype size = m_excludedUsernamesList.size() - 1;
                for (qsizetype i = 0; i < size; ++i) {
                    args.append(QStringLiteral("username<>'%1' AND ").arg(m_excludedUsernamesList.at(i)));
                } //for
                args.append(QStringLiteral("username<>'%1'").arg(m_excludedUsernamesList.at(size)));
            }
        } // (isIncluded || isExcluded)

        const QString request = m_report->selectString().arg(args);
        retVal = m_report->generateReport(request);
        if (!retVal) {
            m_errorString = m_report->errorString();
        }
    } // isIncluded && isExcluded
    m_db.close();

    return retVal;
}

//----------------------------------------------------------

CSVThreadReportBuilder::CSVThreadReportBuilder(QObject *parent)
    : QThread{parent}
{
    m_errorString.clear();
}

bool
CSVThreadReportBuilder::init(quint16 logID, const QString &dbFileName, const QString &reportName,
                             const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList, const bool showMilliseconds)
{
    m_errorString.clear();
    Q_CHECK_PTR(excludedUsernamesList);
    Q_CHECK_PTR(includedUsernamesList);
    bool retVal = m_builder.init(logID, dbFileName, reportName, excludedUsernamesList, includedUsernamesList, showMilliseconds);
    if (!retVal) {
        m_errorString = m_builder.errorString();
    }
    return retVal;
}

void
CSVThreadReportBuilder::run()
{
    m_errorString.clear();
    m_retVal = m_builder.generateReport();
    if (!m_retVal) {
        m_errorString = m_builder.errorString();
    }
}
