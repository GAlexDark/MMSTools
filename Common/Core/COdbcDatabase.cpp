/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: COdbcDatabase.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base ODBC class for work with a databases.
*
****************************************************************************/

#include "COdbcDatabase.h"

bool
COdbcDatabase::init(const QString &connectionString, const QString &name, const QString &password)
{
    bool retVal = CBasicDatabase::init(QLatin1String("QODBC"), connectionString);
    if (retVal) {
        setUserName(name);
        setPassword(password);
    }
    return retVal;
}

bool
COdbcDatabase::init(const QString &connectionString)
{
    return CBasicDatabase::init(QLatin1String("QODBC"), connectionString);
}
