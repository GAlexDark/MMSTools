/****************************************************************************
*
* The classes for generating the MMS Event Log reports.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#include "CReportBuilder.h"
#include "DBStrings.h"
#include "elcUtils.h"
#include "CReportManager.h"

bool
CReportBuilder::configureDb(const QString &dbFileName)
{
    bool retVal = true;
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
    return retVal;
}

bool
CReportBuilder::connectToDatabase(const QString &dbFileName)
{
    bool retVal = m_db.init(dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            retVal = configureDb(dbFileName);
            if (!retVal) {
                m_errorString = m_db.errorString();
            }
        }
    }
    return retVal;
}

bool
CReportBuilder::initReport(const QString &reportName, const bool showMilliseconds)
{
    QStringList tables = m_report->sources();
    bool retVal = !tables.isEmpty();
    if (retVal) {
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
    return retVal;
}

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
    logID = reportManager.getIdByIndex(logID);
    bool retVal = (logID != invalidId);
    if (retVal) {
        m_report = reportManager.getInstance(logID);
        Q_CHECK_PTR(m_report);
        retVal = connectToDatabase(dbFileName);
        if (retVal) {
            retVal = initReport(reportName, showMilliseconds);
        } else {
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
