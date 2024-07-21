/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: COdbcDatabase.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base SQLITE class for work with a database.
*
****************************************************************************/

#ifndef CSQLITEDATABASE_H
#define CSQLITEDATABASE_H

#include "CBasicDatabase.h"

class CSqliteDatabase : public CBasicDatabase
{
public:
    bool init(const QString &connectionString);
    bool optimizeDatabaseSize();

    static bool truncateDB(const QString &connectionString, QString &errorString,
                           qsizetype tablesCount, const QStringList &tablesNames);
    bool truncateTable(const QString &tableName);
    bool checkTables(const QStringList &tables, QString &tableName);
};

using pSqliteDatabase = CSqliteDatabase *;

#endif // CSQLITEDATABASE_H
