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

#ifndef CBASICDATABASE_H
#define CBASICDATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QString>
#include <QMap>
#include <QList>
#include <QScopedPointer>

#include "MMSTypes.h"

using pDataItem = mms::dataItem_t *;
using dataList_t = QList<QStringList>;

class CBasicDatabase
{
public:
    explicit CBasicDatabase(const QString &connectionName);
    CBasicDatabase();
    virtual ~CBasicDatabase();

    const QString& getConnectionName() const { return m_connectionName; }
    CBasicDatabase *getDBinstance() { return this; }
    const QString& errorString() const { return m_errorString; }

    bool open();
    void close();
    void deinit();
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // add info in the DB
    bool prepareRequest(const QString &query);
    bool execRequest(pDataItem data);

    bool insertToDB(const QString &query, pDataItem data);
    // search info in the DB
    dataList_t findInDB(const QString &query, bool addColumnHeaders = true);

    bool exec(const QString &query);

    QVariant geValue(int index) const { return m_SQLRes->value(index); }
    QVariant geValue(const QString &name) const { return m_SQLRes->value(name); }
    bool isNext() { return m_SQLRes->next(); }

private:
    CBasicDatabase(const CBasicDatabase&) = delete;
    CBasicDatabase& operator=(CBasicDatabase&) = delete;

    QSqlDatabase    m_db;
    QScopedPointer<QSqlQuery> m_SQLRes;

    bool            m_isInited = false; //флаг подключения к БД. True - connected, false - not connected
    QString         m_connectionName; //Уникальное имя соединения
    int             m_ErrorCode = 0; //Код ошибки
    QString         m_errorString;

    bool            m_isBeginTransaction = false;

    void _deinit();
    bool _exec(const QString &query);
    bool _exec();

protected:
/*!
    Protected function.
    Inited the DB connection with the DB driver name \a dbDriverName and the connection string \a connectionString.
    If used QSLite, the connectionString is the path to the DB file.
    Returns true on success.
 */
    bool init(const QString &dbDriverName, const QString &connectionString);
    void setUserName(const QString &name);
    void setPassword(const QString &password);
    void sqlQueryFinish();
};

using pBasicDatabase = CBasicDatabase *;

#endif // CBASICDATABASE_H
