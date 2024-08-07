/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2017-2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicDatabase.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for work with a databases.
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
