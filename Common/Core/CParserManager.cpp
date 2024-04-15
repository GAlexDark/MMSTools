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
    m_classList.clear();
    qRegisterMetaType<pEventLogParser>("CEventLogParser");
    m_classList.append(QLatin1String("CEventLogParser")); // ID=1, index=0
    qRegisterMetaType<pAuditTrailParser>("CAuditTrailParser");
    m_classList.append(QLatin1String("CAuditTrailParser")); // ID=2, index=1

    m_visibleLogsNames.resize(m_classList.size());
    m_tablesList.resize(m_classList.size());
    m_createTableRequestList.resize(m_classList.size());

    QString name;
    pBasicParser ptr = nullptr;
    QMetaType type;
    for (qsizetype i = 0; i < m_classList.size(); i++) {
        name = m_classList.at(i).trimmed();
        type = QMetaType::fromName(name.toUtf8());
        if (type.isValid()) {
            ptr = dynamic_cast<pBasicParser>(type.metaObject()->newInstance());
            Q_CHECK_PTR(ptr);
            if (ptr != nullptr) {
                quint16 id = ptr->parserID();
                m_ids.append(id);
                m_visibleLogsNames[id - 1] = ptr->visibleLogName();
                m_tablesList[id - 1] = ptr->tableName();
                m_createTableRequestList[id - 1] = ptr->createTable();

                type.destroy(ptr);
                ptr = nullptr;
            }
        }
    } //for
}
