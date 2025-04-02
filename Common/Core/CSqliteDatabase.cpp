/****************************************************************************
*
* The base class for work with the SQLite database.
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

#include "CSqliteDatabase.h"

bool
CSqliteDatabase::truncateDB(const QString &connectionString, QString &errorString,
                            const QStringList &tablesNames, const QStringList &updateData)
{
    CSqliteDatabase db;
    bool retVal = db.init(connectionString);
    if (retVal) {
        retVal = db.open();
            for (const QString &item : tablesNames) {
                retVal = db.truncateTable(item, updateData);
                if (!retVal) break;
            }
        if (retVal) {
            retVal = db.optimizeDatabaseSize();
        }
        db.close();
    }

    if (!retVal) {
        errorString = db.errorString();
    }
    return retVal;
}

bool
CSqliteDatabase::init(const QString &connectionString)
{
    return CBasicDatabase::init(QLatin1String("QSQLITE"), connectionString);
}

bool
CSqliteDatabase::optimizeDatabaseSize()
{
    sqlQueryFinish();
    return exec(QLatin1String("VACUUM;"));
}

bool
CSqliteDatabase::truncateTable(const QString &tableName, const QStringList &updateData)
{
    sqlQueryFinish();
    bool retVal = false;
    const QString req = QLatin1String("SELECT sql FROM sqlite_master WHERE name = '%1';");
    dataList_t res = findInDB(req.arg(tableName), false);
    if (!res.isEmpty()) {
        retVal = exec(QLatin1String("DROP TABLE IF EXISTS [%1];").arg(tableName));
        if (retVal) {
            QString buffer;
            if (updateData.isEmpty()) {
                buffer = res.at(0).at(0).toLatin1();
            } else {
                QStringList buf = updateData.filter(tableName);
                if (buf.size() == 1) {
                    buffer = buf[0];
                } else {
                    retVal = false;
                }
            }
            if (retVal) {
                retVal = exec(buffer);
            }
        }
    } else {
        retVal = true; // the table does not exist and will be created later
    }

    return retVal;
}

bool
CSqliteDatabase::checkTables(const QStringList &tables, QString &tableName)
{
    bool retVal = true;
    const QString req = QLatin1String("SELECT sql FROM sqlite_master WHERE name = '%1';");
    for (const QString &item : tables) {
        tableName = item;
        dataList_t res = findInDB(req.arg(tableName), false);
        if (res.isEmpty()) {
            retVal = false;
            break;
        }
    }
    return retVal;
}
