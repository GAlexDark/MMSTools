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
*     The base ODBC class for work with a databases.
*
****************************************************************************/

#ifndef CODBCDATABASE_H
#define CODBCDATABASE_H

#include "CBasicDatabase.h"

class COdbcDatabase : public CBasicDatabase
{
public:
    bool init(const QString &connectionString, const QString &name, const QString &password);
    bool init(const QString &connectionString);
};

using pOdbcDatabase = COdbcDatabase *;

#endif // CODBCDATABASE_H
