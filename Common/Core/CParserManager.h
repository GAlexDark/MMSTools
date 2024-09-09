/****************************************************************************
*
* The class for the parsers management.
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

#ifndef CPARSERMANAGER_H
#define CPARSERMANAGER_H

#include <QMetaType>
#include <QVector>
#include "CBasicManager.h"
#include "CBasicParser.h"


/**********************************************************
 * ID=1: Event Log parser and Event Log report
 * ID=2: Audit Trail Log parser and Audit Trail Log report
 *********************************************************/
class CParserManager : public CBasicManager<pBasicParser>
{
public:
    static CParserManager& instance();
    void init();
    const QStringList& getVisibleLogsNames() const { return m_visibleLogsNames; }
    const QStringList& getTablesList() const { return m_tablesList; }
    const QStringList& getCreateTableRequestList() const { return m_createTableRequestList; }

private:
    QStringList     m_visibleLogsNames;
    QStringList     m_tablesList;
    QStringList     m_createTableRequestList;
};

#endif // CPARSERMANAGER_H
