/****************************************************************************
*
* The base class for work with the ODBC database.
* Copyright (C) 2024  Oleksii Gaienko
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
