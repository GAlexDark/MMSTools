/****************************************************************************
*
* The base class for work with a databases.
* Copyright (C) 2017-2024  Oleksii Gaienko
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

#include "CBasicDatabase.h"
#include <QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

void
CBasicDatabase::_deinit()
{
    if (m_isInited) {
        /***************************
         * This source code fixes a standard QSqlDatabase bug:
         * QSqlDatabasePrivate::removeDatabase connection is still in use, all queries will cease to work.
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
    if (!m_SQLRes->exec(query)) {
        m_errorString = QStringLiteral("SQL execution error: %1").arg(m_SQLRes->lastError().text());
        return false;
    }
    return true;
}

bool
CBasicDatabase::_exec()
{
    if (!m_SQLRes->exec()) {
        m_errorString = QStringLiteral("SQL execution error: %1").arg(m_SQLRes->lastError().text());
        return false;
    }
    m_SQLRes->finish();
    return true;
}

void
CBasicDatabase::_connectToDatabase(const QString &dbDriverName, const QString &connectionString)
{
    m_db = QSqlDatabase::addDatabase(dbDriverName, m_connectionName);
    m_isInited = m_db.isValid();
    if (m_isInited) {
        m_db.setDatabaseName(connectionString);
    } else {
        m_errorString = QStringLiteral("Error loading DB driver: %1").arg(m_db.lastError().text());
    }
}

CBasicDatabase::CBasicDatabase(const QString &connectionName)
    : m_connectionName(connectionName.isEmpty() ? QUuid::createUuid().toString() : connectionName)
{
}

CBasicDatabase::CBasicDatabase()
    : m_connectionName(QUuid::createUuid().toString())
{
}

CBasicDatabase::~CBasicDatabase()
{
    deinit();
}

bool
CBasicDatabase::open()
{
    if (m_db.isOpen()) {
        return true;
    }
    if (m_db.open()) {
        m_SQLRes.reset(new QSqlQuery(m_db));
        Q_CHECK_PTR(m_SQLRes);
        if (!m_SQLRes) {
            m_errorString = QStringLiteral("Fatal SQL Query Error.");
            return false;
        }
        return true;
    }

    m_errorString = QStringLiteral("Error open DB file: %1").arg(m_db.lastError().text());
    return false;
}

void
CBasicDatabase::close()
{
    if (m_db.isValid() && m_db.isOpen()) {
        if (m_SQLRes && m_SQLRes->isActive()) {
            m_SQLRes->finish();
        }
        commitTransaction();
        m_db.close();
    }
}

void
CBasicDatabase::deinit()
{
    close();
    _deinit();
}

bool
CBasicDatabase::beginTransaction()
{
    m_isBeginTransaction = m_db.transaction();
    if (!m_isBeginTransaction) {
        m_errorString = QStringLiteral("Transaction Error: %1").arg(m_db.lastError().text());
    }
    return m_isBeginTransaction;
}

bool
CBasicDatabase::commitTransaction()
{
    if (!m_isBeginTransaction) {
        return true;
    }
    if (m_SQLRes && m_SQLRes->isActive()) {
        m_SQLRes->finish();
    }

    if (m_db.commit()) {
        m_isBeginTransaction = false;
        return true;
    }

    m_errorString = QStringLiteral("Transaction Error. Commit status: %1").arg(m_db.lastError().text());
    return false;
}

bool
CBasicDatabase::rollbackTransaction()
{
    if (!m_isBeginTransaction) {
        return true;
    }
    if (!m_db.rollback()) {
        m_errorString = QStringLiteral("Transaction Error. Rollback status: %1").arg(m_db.lastError().text());
        return false;
    }

    m_isBeginTransaction = false;
    return true;
}

bool
CBasicDatabase::prepareRequest(const QString &query)
{
    m_SQLRes->clear();
    if (!m_SQLRes->prepare(query)) {
        m_errorString = QStringLiteral("SQL prepare error: %1").arg(m_SQLRes->lastError().text());
        return false;
    }
    return true;
}

bool
CBasicDatabase::execRequest(const QMap<QString, QVariant> *data)
{
    Q_CHECK_PTR(data);
    for (auto it = data->cbegin(); it != data->cend(); ++it) {
        m_SQLRes->bindValue(it.key(), it.value());
    }
    return _exec();
}

bool
CBasicDatabase::insertToDB(const QString &query, const QMap<QString, QVariant> *data)
{
    Q_CHECK_PTR(data);
    if (!prepareRequest(query)) {
        return false;
    }
    return execRequest(data);
}

dataList_t
CBasicDatabase::findInDB(const QString &query, bool addColumnHeaders)
{
    dataList_t retVal;
    if (exec(query)) {
        QSqlRecord rec = m_SQLRes->record();
        int columnCount = rec.count();
        QStringList item;
        if (addColumnHeaders) {
            for (int i = 0; i < columnCount; ++i) {
                item.append(rec.fieldName(i));
            }
            retVal.append(item);
        }
        item.clear();
        while (m_SQLRes->next()) {
            for (int i = 0; i < columnCount; ++i) {
                item.append(m_SQLRes->value(i).toString().trimmed());
            }
            retVal.append(item);
        }
        m_SQLRes->finish();
    }
    return retVal;
}

bool
CBasicDatabase::exec(const QString &query)
{
    return _exec(query);
}

bool
CBasicDatabase::init(const QString &dbDriverName, const QString &connectionString)
{
    if (m_isInited) {
        return true;
    }
    if (connectionString.isEmpty()) {
        m_errorString = QStringLiteral("Empty connection string");
    } else if (dbDriverName.isEmpty()) {
        m_errorString = QStringLiteral("The DB driver name is empty");
    } else {
        // connecting with the DB
        _connectToDatabase(dbDriverName, connectionString);
    }
    return m_isInited;
}

void
CBasicDatabase::setUserName(const QString &name)
{
    m_db.setUserName(name);
}

void
CBasicDatabase::setPassword(const QString &password)
{
    m_db.setPassword(password);
}

void
CBasicDatabase::sqlQueryFinish()
{
    if (m_SQLRes && m_SQLRes->isActive()) {
        m_SQLRes->finish();
    }
}
