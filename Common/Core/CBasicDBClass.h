#ifndef CBASICDBCLASS_H
#define CBASICDBCLASS_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>

typedef QMap<QString, QVariant> TDataItem; //список полей для bindValue (в формате ":holder_name", "value")
typedef QList<QStringList> TDataList;

class CBasicDBClass
{
public:
    explicit CBasicDBClass(const QString &connectionName = "");
    virtual ~CBasicDBClass();

    QString getConnectionName() const { return m_connectionName; }
    CBasicDBClass *getDBinstance() { return this; }
    QString errorString() const { return m_errorString; }

    bool init(const QString &dbName, const QString &connectionString);
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

    QVariant geValue(int index) const { return m_SQLRes.value(index); }
    bool isNext() { return m_SQLRes.next(); }

private:
    //работа с БД
    QSqlDatabase    m_db;
    QSqlQuery       m_SQLRes;

    bool            m_isInited;         //флаг подключения к БД. True - connected, false - not connected
    QString         m_connectionName;   //Уникальное имя соединения
    int             m_ErrorCode;        //Код ошибки
    QString         m_errorString;

    bool            m_isBeginTransaction;

    void _Close();
    void _Deinit();
    bool _exec(const QString &query);
    bool _exec();
};

#endif // CBASICDBCLASS_H
