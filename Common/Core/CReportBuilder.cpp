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
    const int blockSize = elcUtils::getStorageBlockSize(dbFileName);
    const QStringList pragmaItems = { pragmaUTF8, pragmaPageSize.arg(blockSize) };
    for (const QString &item : std::as_const(pragmaItems)) {
        if (!m_db.exec(item)) {
            return false;
        }
    }
    return true;
}

bool
CReportBuilder::connectToDatabase(const QString &dbFileName)
{
    if (!m_db.init(dbFileName) || !m_db.open()) {
        return false;
    }
    if (!configureDb(dbFileName)) {
        m_errorString = m_db.errorString();
        return false;
    }
    return true;
}

bool
CReportBuilder::initReport(const QString &reportName, const bool showMilliseconds)
{
    const QStringList tables = m_report->sources();
    if (tables.isEmpty()) {
        m_errorString = QStringLiteral("List of tables not found. The report cannot check them.");
        return false;
    }
    QString table;
    if (!m_db.checkTables(tables, table)) {
        m_errorString = QStringLiteral("The table '%1' not found. The report cannot create results.").arg(table);
        return false;
    }
    m_report->init(&m_db, reportName, showMilliseconds);
    return true;
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
    Q_CHECK_PTR(includedUsernamesList);

    m_excludedUsernamesList = *excludedUsernamesList;
    m_includedUsernamesList = *includedUsernamesList;

    CReportManager &reportManager = CReportManager::instance();
    logID = reportManager.getIdByIndex(logID);

    if (logID == invalidId) {
        m_errorString = QStringLiteral("The report not found");
        return false;
    }

    m_report = reportManager.getInstance(logID);
    Q_CHECK_PTR(m_report);

    if (!connectToDatabase(dbFileName) || !initReport(reportName, showMilliseconds)) {
        m_db.close();
        return false;
    }
    return true;
}

bool
CReportBuilder::generateReport()
{
    bool isIncluded = !m_includedUsernamesList.isEmpty();
    bool isExcluded = !m_excludedUsernamesList.isEmpty();

    if (isIncluded && isExcluded) {
        m_errorString = QStringLiteral("User exclusion and inclusion lists cannot be specified at the same time.");
        return false;
    }

    QString args;
    if (isIncluded) {
        qsizetype size = m_includedUsernamesList.size() - 1;
        for (qsizetype i = 0; i < size; ++i) {
            args.append(QStringLiteral("username='%1' OR ").arg(m_includedUsernamesList.at(i)));
        }
        args.append(QStringLiteral("username='%1'").arg(m_includedUsernamesList.at(size)));
        args = "WHERE " + args;
    } else if (isExcluded) {
        qsizetype size = m_excludedUsernamesList.size() - 1;
        for (qsizetype i = 0; i < size; ++i) {
            args.append(QStringLiteral("username<>'%1' AND ").arg(m_excludedUsernamesList.at(i)));
        }
        args.append(QStringLiteral("username<>'%1'").arg(m_excludedUsernamesList.at(size)));
        args = "WHERE " + args;
    }

    const QString request = m_report->selectString().arg(args);
    if (!m_report->generateReport(request)) {
        m_errorString = m_report->errorString();
        m_db.close();
        return false;
    }

    m_db.close();
    return true;
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

    if (!m_builder.init(logID, dbFileName, reportName, excludedUsernamesList, includedUsernamesList, showMilliseconds)) {
        m_errorString = m_builder.errorString();
        return false;
    }
    return true;
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
