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
