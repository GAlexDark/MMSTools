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
    QStringList getVisibleLogsNames() const { return m_visibleLogsNames; }
    QStringList getTablesList() const { return m_tablesList; }
    QStringList getCreateTableRequestList() const { return m_createTableRequestList; }

private:
    QStringList     m_visibleLogsNames;
    QStringList     m_tablesList;
    QStringList     m_createTableRequestList;
};

#endif // CPARSERMANAGER_H
