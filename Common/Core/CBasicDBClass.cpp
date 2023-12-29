#include "CBasicDBClass.h"
#include <QUuid>
#include <QSqlError>
#include <QSqlRecord>
#include "Debug.h"

void
CBasicDBClass::_Close()
{
    __DEBUG( Q_FUNC_INFO )

    if (m_db.isOpen()) {
        __DEBUG( "DB is open" )
        if (m_SQLRes.isActive()) {
            m_SQLRes.finish();
        }
        m_db.close();
    }
}

void
CBasicDBClass::_Deinit()
{
    __DEBUG( Q_FUNC_INFO )

    // код избавления от бага:
    // QSqlDatabasePrivate::removeDatabase connection is still in use, all queries will cease to work.
    if (m_isInited) {
        QString qs;
        qs.append(QSqlDatabase::database().connectionName());
        __DEBUG( QStringLiteral("connectionName: %1").arg(qs) )
        QSqlDatabase::removeDatabase(qs);
        __DEBUG( "Database removed" )
        //конец кода
        m_isInited = false;
    }
}

bool
CBasicDBClass::_exec(const QString &query)
{
    //__DEBUG( Q_FUNC_INFO )

    m_SQLRes.clear();
    m_SQLRes.setForwardOnly(true); // ускорение для ::next()
    bool retVal = m_SQLRes.exec(query);
    if (!retVal) {
        QSqlError error = m_SQLRes.lastError();
        m_errorString = QStringLiteral("SQL execution error: %1").arg(error.text());
        __DEBUG(m_errorString)
    }
    return retVal;
}

bool
CBasicDBClass::_exec()
{
    //__DEBUG( Q_FUNC_INFO )

    bool retVal = m_SQLRes.exec();
    if (!retVal) {
        QSqlError error = m_SQLRes.lastError();
        m_errorString = QStringLiteral("SQL execution error: %1").arg(error.text());
        __DEBUG(m_errorString)
    }
    return retVal;
}

/*********************************************************************
 *
 *  Public functions
 *
 ********************************************************************/

CBasicDBClass::CBasicDBClass(const QString &connectionName): m_isInited(false), m_connectionName(connectionName),
    m_ErrorCode(0), m_isBeginTransaction(false)
{
    __DEBUG( Q_FUNC_INFO )

    if (m_connectionName.isEmpty()) {
        QUuid guid = QUuid::createUuid();
        m_connectionName = guid.toString();
    }
}

CBasicDBClass::~CBasicDBClass()
{
    __DEBUG( Q_FUNC_INFO )
    _Deinit();
}

/**
 * @brief init
 * @param dbDriverName - имя драйвера БД
 * @param connectionString - для SQLite это путь к файлу БД
 */
bool
CBasicDBClass::init(const QString &dbDriverName, const QString &connectionString)
{
    __DEBUG( Q_FUNC_INFO )

    if (!m_isInited) {
        if (connectionString.isEmpty() || dbDriverName.isEmpty()) {
            m_isInited = false;
            m_errorString = QStringLiteral("Empty connection string");
            __DEBUG( m_errorString )
        } else {
            // соединяемся с базой данных
            m_db = QSqlDatabase::addDatabase(dbDriverName, m_connectionName);
            m_isInited = m_db.isValid();
            if (m_isInited) {
                m_db.setDatabaseName(connectionString);
            } else {
                QSqlError error = m_db.lastError();
                m_errorString = QStringLiteral("Error loading DB driver: %1").arg(error.text());
                __DEBUG( m_errorString );
            }
        }
    }
    return m_isInited;
}
/*
    закончить транзакции (commit)
    закрыть подключение к базе
    деинициализация подключения
*/

bool
CBasicDBClass::open()
{
    __DEBUG( Q_FUNC_INFO )

    bool retVal = true;
    if(!m_db.isOpen()) {
        retVal = m_db.open();
        if (retVal) {
            m_SQLRes = QSqlQuery(m_db); // <-- единое связывание всех запросов
        } else {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Error open DB file: %1").arg(error.text());
            __DEBUG( m_errorString )
        }
    }
    return retVal;
}

void
CBasicDBClass::close()
{
    __DEBUG( Q_FUNC_INFO )

    if (m_db.isValid() && m_db.isOpen()) {
        if (m_SQLRes.isActive()) {
            __DEBUG( "m_SQLRes is active" );
            m_SQLRes.finish();
        }
        __DEBUG( "DB is open" )
        commitTransaction();
        __DEBUG( "Closing DB" )
        m_db.close();
        __DEBUG( "DB was closed" )
    }
}

void
CBasicDBClass::deinit()
{
    __DEBUG( Q_FUNC_INFO )

    _Close();
    _Deinit();
}

bool
CBasicDBClass::beginTransaction()
{
    __DEBUG( Q_FUNC_INFO )

    m_isBeginTransaction = m_db.transaction();
    if (!m_isBeginTransaction) {
        QSqlError error = m_db.lastError();
        m_errorString = QStringLiteral("Transaction Error: %1").arg(error.text());
        __DEBUG( m_errorString )
    }
    return m_isBeginTransaction;
}

bool
CBasicDBClass::commitTransaction()
{
    __DEBUG( Q_FUNC_INFO )

    if (m_SQLRes.isActive()) {
        __DEBUG( "m_SQLRes is active" );
        m_SQLRes.finish();
        }

    bool retVal = true;
    if (m_isBeginTransaction) {
        __DEBUG( "Transaction is active " );
        retVal = m_db.commit();
        if (retVal) {
            m_isBeginTransaction = false;
        } else {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Transaction Error. Commit status: %1").arg(error.text());
            __DEBUG(m_errorString)
        }
    }
    return retVal;
}

bool
CBasicDBClass::rollbackTransaction()
{     
    __DEBUG( Q_FUNC_INFO )

    if (m_SQLRes.isActive()) {
        __DEBUG( "isActive = true" )
        m_SQLRes.finish();
    }

    bool retVal = true;
    if (m_isBeginTransaction) {
        retVal = m_db.rollback();
        if (retVal) {
        } else {
            QSqlError error = m_db.lastError();
            m_errorString = QStringLiteral("Transaction Error. Rollback status: %1").arg(error.text());
            __DEBUG(m_errorString)
        }
        m_isBeginTransaction = false;
    }
    return retVal;
}

bool
CBasicDBClass::optimizeDatabaseSize()
{
    if (m_SQLRes.isActive()) {
        m_SQLRes.finish();
    }
    return _exec(QStringLiteral("VACUUM;"));
}

bool
CBasicDBClass::truncateTable(const QString &tableName)
{
    if (m_SQLRes.isActive()) {
        m_SQLRes.finish();
    }
    bool retVal = _exec(QStringLiteral("drop table if exists [%1];").arg(tableName));
    if (retVal) {
        retVal = optimizeDatabaseSize();
    }
    return retVal;
}

bool
CBasicDBClass::prepareRequest(const QString &query)
{
    __DEBUG( Q_FUNC_INFO )

	m_SQLRes.clear();
    bool retVal = m_SQLRes.prepare(query);
    if (!retVal) {
		QSqlError error = m_SQLRes.lastError();
        m_errorString = QStringLiteral("SQL prepare error: %1").arg(error.text());
        __DEBUG(m_errorString)
    }
    return retVal;
}

bool
CBasicDBClass::execRequest(TDataItem data)
{
    TDataItem::iterator itStart = data.begin();
    TDataItem::iterator itEnd = data.end();

    while (itStart != itEnd) {
        m_SQLRes.bindValue (itStart.key(), itStart.value());
        ++itStart;
    }

    bool retVal = _exec();
    m_SQLRes.finish();
    return retVal;
}

bool
CBasicDBClass::insertToDB(const QString &query, TDataItem data)
{
    bool retVal = prepareRequest(query);
    if (retVal) {
        TDataItem::iterator itStart = data.begin();
        TDataItem::iterator itEnd = data.end();

        while (itStart != itEnd) {
            m_SQLRes.bindValue (itStart.key(), itStart.value());
            ++itStart;
        }
        retVal = _exec();
        m_SQLRes.finish();
    }
    return retVal;
}

TDataList
CBasicDBClass::findInDB(const QString &query, bool addColumnHeaders)
{
    bool hasResult = false;
    TDataList retVal;
    if (exec(query)) {
        QStringList item;

        QSqlRecord rec = m_SQLRes.record();
        int columnCount = rec.count();
        __DEBUG( "Column count: " + QString::number(columnCount) )

        if (addColumnHeaders) {
            __DEBUG( "Headers enabled" )
            for (int i = 0; i < columnCount; i++) {
                item.append(rec.fieldName(i));
            }
            retVal.append(item);
        }

        while(m_SQLRes.next()) {
            hasResult = true;
            item.clear();
            for (int i = 0; i < columnCount; i++) {
                item.append(m_SQLRes.value(i).toString().trimmed());
            }
            retVal.append(item);
        }
        m_SQLRes.finish();
    }
    return (hasResult)? retVal : TDataList();
}

bool
CBasicDBClass::exec(const QString &query)
{
    bool retVal = _exec(query);
    return retVal;
}
