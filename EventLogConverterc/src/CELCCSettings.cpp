/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CELCCSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance of the
*     Event Log Conversion Console Utility.
*
****************************************************************************/

#include "CELCCSettings.h"

static CELCCSettings g_elccSettings;

CELCCSettings& CELCCSettings::instance()
{
    return g_elccSettings;
}
/*
void
CELCCSettings::createDefault(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);

    settings.beginGroup(QStringLiteral("SETTINGS"));
#ifdef QT_DEBUG
    settings.setValue(QStringLiteral("db_file_name"), QStringLiteral(TEST_SRCDIR"EventLogConverter.db"));
#else
    settings.setValue(QStringLiteral("db_file_name"), QStringLiteral("EventLogConverter.db"));
#endif
    settings.setValue(QStringLiteral("clear_on_startup"), QStringLiteral("yes")); // yes | no
    settings.setValue(QStringLiteral("internal_ip_start_octet"), QStringLiteral("10."));
    settings.endGroup();

    settings.beginGroup(QStringLiteral("DATABASE"));
    settings.setValue(QStringLiteral("temp_store"), QStringLiteral("MEMORY"));
    settings.setValue(QStringLiteral("journal_mode"), QStringLiteral("MEMORY"));
    settings.endGroup();
}
*/
