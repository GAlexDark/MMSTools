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

static CParserManager g_parserManager;

CParserManager& CParserManager::instance()
{
    return g_parserManager;
}

void
CParserManager::init()
{
    qRegisterMetaType<pEventLogParser>("CEventLogParser");
    m_classList.append(QLatin1String("CEventLogParser")); // ID=1, index=0
    qRegisterMetaType<pAuditTrailParser>("CAuditTrailParser");
    m_classList.append(QLatin1String("CAuditTrailParser")); // ID=2, index=1

    m_visibleLogsNames.resize(m_classList.size());
    m_tablesList.resize(m_classList.size());
    m_createTableRequestList.resize(m_classList.size());

    pBasicParser ptr = nullptr;
    QMetaType type;
    for (const QString &name : m_classList) {
        type = QMetaType::fromName(name.toUtf8());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicParser>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            quint16 id = ptr->parserID();
            m_ids.append(id);
            m_visibleLogsNames[id - 1] = ptr->visibleLogName();
            m_tablesList[id - 1] = ptr->tableName();
            m_createTableRequestList[id - 1] = ptr->createTable();

            type.destroy(ptr);
            ptr = nullptr;
        }
    }
}
