/****************************************************************************
 *
 *  Copyright (c) Oleksii Gaienko, 2024
 *  Contact: galexsoftware@gmail.com
 *
 *  Event Log Conversion Utility
 *  Common module
 *
 *  Module name: CParserManager.h
 *  Author(s): Oleksii Gaienko
 *  Reviewer(s):
 *
 *  Abstract:
 *     The class for parser management
 *
 ****************************************************************************/

#ifndef CPARSERMANAGER_H
#define CPARSERMANAGER_H

#include "CBasicManager.h"
#include "CBasicParser.h"
#include <QMetaType>
#include <QVector>

/**********************************************************
 * ID=1: Event Log parser and Event Log report
 * ID=2: Audit Trail Log parser and Audit Trail Log report
 *********************************************************/
class CParserManager : public CBasicManager<pBasicParser> {
public:
  static CParserManager &instance();
  void init();
  QStringList getVisibleLogsNames() const { return m_visibleLogsNames; }
  QStringList getTablesList() const { return m_tablesList; }
  QStringList getCreateTableRequestList() const {
    return m_createTableRequestList;
  }

private:
  QStringList m_visibleLogsNames;
  QStringList m_tablesList;
  QStringList m_createTableRequestList;
};

#endif // CPARSERMANAGER_H
