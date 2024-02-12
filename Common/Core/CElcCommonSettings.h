/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CElcCommonSettings.n
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for creating default settings in the ini file.
*
****************************************************************************/

#ifndef CELCCOMMONSETTINGS_H
#define CELCCOMMONSETTINGS_H
#include "CBasicSettings.h"

inline const QString SettingsDbFileName = QStringLiteral("SETTINGS/db_file_name");
inline const QString KeyDbFileName = QStringLiteral("db_file_name");
inline const QString SettingsClearOnStartup = QStringLiteral("SETTINGS/clear_on_startup");
inline const QString SettingsInternalIpStartOctet = QStringLiteral("SETTINGS/internal_ip_start_octet");
inline const QString DatabaseSynchronous = QStringLiteral("DATABASE/synchronous");
inline const QString DatabaseJournalMode = QStringLiteral("DATABASE/journal_mode");
inline const QString DatabaseTempStore = QStringLiteral("DATABASE/temp_store");
inline const QString DatabaseLockingMode = QStringLiteral("DATABASE/locking_mode");

inline const QString SettingsGroup = QStringLiteral("SETTINGS");
inline const QString DatabaseGroup = QStringLiteral("DATABASE");

class CElcCommonSettings : public CBasicSettings
{
protected:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCCOMMONSETTINGS_H
