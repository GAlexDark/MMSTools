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

#include "CParserManager.h"
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QMetaType>
#include <QMap>
#include <QString>
#include <QStringList>

#include "CEventLogParser.h"
#include "CEventLogParser2.h"
#include "CAuditTrailParser.h"
#include "CSystemLogParser.h"

CParserManager& CParserManager::instance()
{
    static CParserManager g_parserManager;
    return g_parserManager;
}

void
CParserManager::init()
{
    qRegisterMetaType<pEventLogParser>("CEventLogParser");
    addClassListItem(QLatin1String("CEventLogParser")); // ID=1
    qRegisterMetaType<pAuditTrailParser>("CAuditTrailParser");
    addClassListItem(QLatin1String("CAuditTrailParser")); // ID=2
    qRegisterMetaType<pSystemLogParser>("CSystemLogParser");
    addClassListItem(QLatin1String("CSystemLogParser")); // ID=3
    qRegisterMetaType<pEventLogParser2>("CEventLogParser2");
    addClassListItem(QLatin1String("CEventLogParser2")); // ID=4

    struct parserData_t
    {
        QString visibleLogName;
        QString tableName;
        QString createTableString;
    };

    QMap<quint16, parserData_t> parserNameMap;
    pBasicParser ptr = nullptr;
    QMetaType type;
    QStringList classList = getClassList();
    for (const QString &name : classList) {
        type = QMetaType::fromName(name.toUtf8().constData());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicParser>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            quint16 id = ptr->parserID();
            addId(id);

            parserNameMap[id] = {ptr->visibleLogName(), ptr->tableName(), ptr->createTable()};

            type.destroy(ptr);
            ptr = nullptr;
        }
    }

    sortIds();
    for (const quint16 i: getIds()) {
        m_visibleLogsNames.append(parserNameMap[i].visibleLogName);
        m_tablesList.append(parserNameMap[i].tableName);
        m_createTableRequestList.append(parserNameMap[i].createTableString);
    }
    m_tablesList.removeDuplicates();
    m_createTableRequestList.removeAll("");
}
