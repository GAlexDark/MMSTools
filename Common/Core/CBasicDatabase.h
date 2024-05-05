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
*     The base class for work with a database.
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

typedef QMap<QString, QVariant> dataItem_t;
typedef dataItem_t *pDataItem;
typedef QList<QStringList> dataList_t;

class CBasicDatabase
{
public:
    explicit CBasicDatabase(const QString &connectionName);
    CBasicDatabase();
    ~CBasicDatabase();

    QString getConnectionName() const { return m_connectionName; }
    CBasicDatabase *getDBinstance() { return this; }
    QString errorString() const { return m_errorString; }

/*!
 * bool CBasicDatabase::init(const QString &dbDriverName, const QString &connectionString).
 * Inited the DB connection with the DB driver name \a dbDriverName and the \a connectionString.
 * If used QSLite, the \a connectionString is the path to the DB file.
 */
    bool init(const QString &dbDriverName, const QString &connectionString);
    bool open();
    void close();
    void deinit();
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    bool optimizeDatabaseSize();
    bool truncateTable(const QString &tableName);
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

    bool            m_isInited = false;         //флаг подключения к БД. True - connected, false - not connected
    QString         m_connectionName;   //Уникальное имя соединения
    int             m_ErrorCode = 0;        //Код ошибки
    QString         m_errorString;

    bool            m_isBeginTransaction = false;

    void _Deinit();
    bool _exec(const QString &query);
    bool _exec();
};

typedef CBasicDatabase *pBasicDatabase;

#endif // CBASICDATABASE_H
