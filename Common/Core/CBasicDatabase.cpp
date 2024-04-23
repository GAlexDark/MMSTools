/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2017-2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicDatabase.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for work with a database.
*
****************************************************************************/

#include "CBasicDatabase.h"
#include <QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

//#include "Debug.h"

void
CBasicDatabase::_Deinit()
{
    if (m_isInited) {
/***************************
 * This sourse code fixed a standard QSqlDatabase bug:
 *   QSqlDatabasePrivate::removeDatabase connection is still in use, all queries will cease to work.
 **************************/
        const QString connectionName = m_db.connectionName();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);

        m_isInited = false;
    }
}

bool
CBasicDatabase::_exec(const QString &query)
{
    m_SQLRes->clear();
    m_SQLRes->setForwardOnly(true); // ускорение для ::next()
    bool retVal = m_SQLRes->exec(query);
    if (!retVal) {
        QSqlError error = m_SQLRes->lastError();
        m_errorString = QStringLiteral("SQL execution error: %1").arg(error.text());
    }
    return retVal;
}

bool
CBasicDatabase::_exec()
{
    bool retVal = m_SQLRes->exec();
    if (!retVal) {
        QSqlError error = m_SQLRes->lastError();
        m_errorString = QStringLiteral("SQL execution error: %1").arg(error.text());
    }
    return retVal;
}

/*********************************************************************
 *
 *  Public functions
 *
 ********************************************************************/

CBasicDatabase::CBasicDatabase(const QString &connectionName)
    : m_SQLRes(nullptr), m_isInited(false), m_connectionName(connectionName), m_ErrorCode(0), m_isBeginTransaction(false)
{
    if (m_connectionName.isEmpty()) {
        QUuid guid = QUuid::createUuid();
        m_connectionName = guid.toString();
    }
}

CBasicDatabase::CBasicDatabase()
    : m_SQLRes(nullptr), m_isInited(false), m_connectionName(""), m_ErrorCode(0), m_isBeginTransaction(false)
{
    QUuid guid = QUuid::createUuid();
    m_connectionName = guid.toString();
}

CBasicDatabase::~CBasicDatabase()
{
    close();
    _Deinit();
}

bool
CBasicDatabase::init(const QString &dbDriverName, const QString &connectionString)
{
    if (!m_isInited) {
        if (connectionString.isEmpty() || dbDriverName.isEmpty()) {
            m_isInited = false;
            m_errorString = QStringLiteral("Empty connection string");
        } else {
            // соединяемся с базой данных
            m_db = QSqlDatabase::addDatabase(dbDriverName, m_connectionName);
            m_isInited = m_db.isValid();
            if (m_isInited) {
                m_db.setDatabaseName(connectionString);
            } else {
                QSqlError error = m_db.lastError();
                m_errorString = QStringLiteral("Error loading DB driver: %1").arg(error.text());
            }
        }
    }
    return m_isInited;
}

bool
CBasicDatabase::open()
{
    bool retVal = true;
    if (!m_db.isOpen()) {
        retVal = m_db.open();
        if (retVal) {
            try {
                m_SQLRes = new QSqlQuery(m_db);
                Q_CHECK_PTR(m_SQLRes);
            } catch (const std::bad_alloc &e) {
                m_errorString = QStringLiteral("Fatal SQL Query Error: %1").arg(e.what());
                retVal = false;
            }
        } else {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Error open DB file: %1").arg(error.text());
        }
    }
    return retVal;
}

void
CBasicDatabase::close()
{
    if (m_db.isValid() && m_db.isOpen()) {
        if (m_SQLRes && m_SQLRes->isActive()) {
            m_SQLRes->finish();
            delete m_SQLRes;
            m_SQLRes = nullptr;
        }
        commitTransaction();
        m_db.close();
    }
}

void
CBasicDatabase::deinit()
{
    close();
    _Deinit();
}

bool
CBasicDatabase::beginTransaction()
{
    m_isBeginTransaction = m_db.transaction();
    if (!m_isBeginTransaction) {
        QSqlError error = m_db.lastError();
        m_errorString = QStringLiteral("Transaction Error: %1").arg(error.text());
    }
    return m_isBeginTransaction;
}

bool
CBasicDatabase::commitTransaction()
{
    bool retVal = true;
    if (m_isBeginTransaction) {
        if (m_SQLRes && m_SQLRes->isActive()) {
            m_SQLRes->finish();
        }

        retVal = m_db.commit();
        if (retVal) {
            m_isBeginTransaction = false;
        } else {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Transaction Error. Commit status: %1").arg(error.text());
        }
    }
    return retVal;
}

bool
CBasicDatabase::rollbackTransaction()
{     
    bool retVal = true;
    if (m_isBeginTransaction) {
        retVal = m_db.rollback();
        if (!retVal) {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Transaction Error. Rollback status: %1").arg(error.text());
        }
        m_isBeginTransaction = false;
    }
    return retVal;
}

bool
CBasicDatabase::optimizeDatabaseSize()
{
    if (m_SQLRes->isActive()) {
        m_SQLRes->finish();
    }
    return _exec(QLatin1String("VACUUM;"));
}

bool
CBasicDatabase::truncateTable(const QString &tableName)
{
    if (m_SQLRes->isActive()) {
        m_SQLRes->finish();
    }
    bool retVal = _exec(QLatin1String("drop table if exists [%1];").arg(tableName));
    if (retVal) {
        retVal = optimizeDatabaseSize();
    }
    return retVal;
}

bool
CBasicDatabase::prepareRequest(const QString &query)
{
    m_SQLRes->clear();
    bool retVal = m_SQLRes->prepare(query);
    if (!retVal) {
        QSqlError error = m_SQLRes->lastError();
        m_errorString = QStringLiteral("SQL prepare error: %1").arg(error.text());
    }
    return retVal;
}

bool
CBasicDatabase::execRequest(pDataItem data)
{
    Q_CHECK_PTR(data);
    auto itStart = data->begin();
    auto itEnd = data->end();

    while (itStart != itEnd) {
        m_SQLRes->bindValue (itStart.key(), itStart.value());
        ++itStart;
    }

    bool retVal = _exec();
    return retVal;
}

bool
CBasicDatabase::insertToDB(const QString &query, pDataItem data)
{
    Q_CHECK_PTR(data);
    bool retVal = prepareRequest(query);
    if (retVal) {
        auto itStart = data->begin();
        auto itEnd = data->end();

        while (itStart != itEnd) {
            m_SQLRes->bindValue (itStart.key(), itStart.value());
            ++itStart;
        }
        retVal = _exec();
        m_SQLRes->finish();
    }
    return retVal;
}

dataList_t
CBasicDatabase::findInDB(const QString &query, bool addColumnHeaders)
{
    bool hasResult = false;
    dataList_t retVal;
    if (exec(query)) {
        QStringList item;

        QSqlRecord rec = m_SQLRes->record();
        int columnCount = rec.count();
        if (addColumnHeaders) {
            for (int i = 0; i < columnCount; i++) {
                item.append(rec.fieldName(i));
            }
            retVal.append(item);
        }

        while(m_SQLRes->next()) {
            hasResult = true;
            item.clear();
            for (int i = 0; i < columnCount; i++) {
                item.append(m_SQLRes->value(i).toString().trimmed());
            }
            retVal.append(item);
        }
        m_SQLRes->finish();
    }
    return (hasResult)? retVal : dataList_t();
}

bool
CBasicDatabase::exec(const QString &query)
{
    bool retVal = _exec(query);
    return retVal;
}
