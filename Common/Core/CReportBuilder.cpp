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
    : m_db(nullptr), m_report(nullptr), m_errorString("")
{}

CReportBuilder::~CReportBuilder()
{
    delete m_db;
    m_db = nullptr;

    m_includedUsernamesList.clear();
    m_excludedUsernamesList.clear();
}

bool
CReportBuilder::init(quint16 logID, const QString &dbFileName, const QString &reportName,
                    const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList)
{
    Q_CHECK_PTR(excludedUsernamesList);
    m_excludedUsernamesList = *excludedUsernamesList;
    Q_CHECK_PTR(includedUsernamesList);
    m_includedUsernamesList = *includedUsernamesList;

    CReportManager &reportManager = CReportManager::instance();
    m_report = nullptr;
    bool retVal = reportManager.checkID(logID);
    if (retVal) {
        m_report = reportManager.getInstance(logID);
        Q_CHECK_PTR(m_report);

        try {
            m_db = new CBasicDatabase();
            Q_CHECK_PTR(m_db);
        } catch (const std::bad_alloc &e) {
            retVal = false;
            m_errorString = e.what();
        }

        if (retVal) {
            retVal = m_db->init(QLatin1String("QSQLITE"), dbFileName);
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
            if (retVal) {
                m_report->init(m_db, reportName);
            } else {
                m_errorString = m_db->errorString();
                m_db->close();
            }
        }
    } else {
        m_errorString = QStringLiteral("The parser not found");
    }

    return retVal;
}

bool
CReportBuilder::generateReport()
{
    QString args;
    args.clear();
    if (m_includedUsernamesList.isEmpty()) {
        if (!m_excludedUsernamesList.isEmpty()) {
            args.append(QLatin1String("WHERE "));
            qsizetype size = m_excludedUsernamesList.size() - 1;
            for (qsizetype i = 0; i < size; ++i) {
                args.append(QStringLiteral("e.username<>'%1' AND ").arg(m_excludedUsernamesList.at(i)));
            } //for
            args.append(QStringLiteral("e.username<>'%1'").arg(m_excludedUsernamesList.at(size)));
        }
    } else {
        args.append(QLatin1String("WHERE "));
        qsizetype size = m_includedUsernamesList.size() - 1;
        for (qsizetype i = 0; i < size; ++i) {
            args.append(QStringLiteral("e.username='%1' OR ").arg(m_includedUsernamesList.at(i)));
        } //for
        args.append(QStringLiteral("e.username='%1'").arg(m_includedUsernamesList.at(size)));
    }
    bool retVal = m_report->generateReport(args);
    if (!retVal) {
        m_errorString = m_report->errorString();
    }

    m_db->close();

    return retVal;
}

//----------------------------------------------------------

CSVThreadReportBuilder::CSVThreadReportBuilder(QObject *parent)
    : QThread(parent), m_errorString(""), m_retVal(false)
{}

bool
CSVThreadReportBuilder::init(quint16 logID, const QString &dbFileName, const QString &reportName,
                                  const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList)
{
    Q_CHECK_PTR(excludedUsernamesList);
    Q_CHECK_PTR(includedUsernamesList);
    return m_builder.init(logID, dbFileName, reportName, excludedUsernamesList, includedUsernamesList);
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

