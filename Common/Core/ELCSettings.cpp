/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  cmdbsmallparser
*  cmdbsmallparser
*
*  Module name: ELCSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Basic slass for read-write ini-settings
*
****************************************************************************/

#include "ELCSettings.h"

static ELCSettings g_elcSettings;

ELCSettings& ELCSettings::instance()
{
    return g_elcSettings;
}

void
ELCSettings::createDefault(const QString& iniPath)
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
