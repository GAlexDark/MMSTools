/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CParserManager.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for parser management
*
****************************************************************************/

#include "CParserManager.h"
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QMetaType>

#include "CEventLogParser.h"
#include "CAuditTrailParser.h"
#include "CSystemLogParser.h"

static CParserManager g_parserManager;

CParserManager& CParserManager::instance()
{
    return g_parserManager;
}

void
CParserManager::init()
{
    qRegisterMetaType<pEventLogParser>("CEventLogParser");
    m_classList.append(QLatin1String("CEventLogParser")); // ID=1
    qRegisterMetaType<pAuditTrailParser>("CAuditTrailParser");
    m_classList.append(QLatin1String("CAuditTrailParser")); // ID=2
    qRegisterMetaType<pSystemLogParser>("CSystemLogParser");
    m_classList.append(QLatin1String("CSystemLogParser")); // ID=3

    struct parserData_t
    {
        QString visibleLogName;
        QString tableName;
        QString createTableString;
    };

    QMap<quint16, parserData_t> parserNameMap;
    pBasicParser ptr = nullptr;
    QMetaType type;
    for (const QString &name : m_classList) {
        type = QMetaType::fromName(name.toUtf8());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicParser>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            quint16 id = ptr->parserID();
            m_ids.append(id);

            parserNameMap[id].visibleLogName = ptr->visibleLogName();
            parserNameMap[id].tableName = ptr->tableName();
            parserNameMap[id].createTableString = ptr->createTable();

            type.destroy(ptr);
            ptr = nullptr;
        }
    }

    std::sort(m_ids.begin(), m_ids.end());
    for (const quint16 i: m_ids) {
        m_visibleLogsNames.append(parserNameMap[i].visibleLogName);
        m_tablesList.append(parserNameMap[i].tableName);
        m_createTableRequestList.append(parserNameMap[i].createTableString);
    }
}
