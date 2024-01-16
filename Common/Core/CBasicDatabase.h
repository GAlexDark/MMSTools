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


typedef QMap<QString, QVariant> TDataItem; //список полей для bindValue (в формате ":holder_name", "value")
typedef QList<QStringList> TDataList;

class CBasicDatabase
{
public:
    explicit CBasicDatabase(const QString &connectionName);
    CBasicDatabase();
    virtual ~CBasicDatabase();

    QString getConnectionName() const { return m_connectionName; }
    CBasicDatabase *getDBinstance() { return this; }
    QString errorString() const { return m_errorString; }

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
    bool execRequest(TDataItem data);

    bool insertToDB(const QString &query, TDataItem data);
    // search info in the DB
    TDataList findInDB(const QString &query, bool addColumnHeaders = true);

    bool exec(const QString &query);

    QVariant geValue(int index) const { return m_SQLRes->value(index); }
    bool isNext() { return m_SQLRes->next(); }

private:
    CBasicDatabase(const CBasicDatabase&) = delete;
    CBasicDatabase& operator=(CBasicDatabase&) = delete;

    QSqlDatabase    m_db;
    QSqlQuery       *m_SQLRes;

    bool            m_isInited;         //флаг подключения к БД. True - connected, false - not connected
    QString         m_connectionName;   //Уникальное имя соединения
    int             m_ErrorCode;        //Код ошибки
    QString         m_errorString;

    bool            m_isBeginTransaction;

    void _Deinit();
    bool _exec(const QString &query);
    bool _exec();
};

#endif // CBASICDATABASE_H
