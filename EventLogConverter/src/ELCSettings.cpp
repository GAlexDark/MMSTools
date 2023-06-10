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

ELCSettings::ELCSettings()
{

}

void
ELCSettings::createDefault(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);

    settings.beginGroup(QStringLiteral("SETTINGS"));
#ifdef QT_DEBUG
    settings.setValue(QStringLiteral("db_file_name"), "E:\\PROJECTS\\Qt\\MMSTools\\test_source\\EventLogConverter.db");
#else
    settings.setValue(QStringLiteral("db_file_name"), "EventLogConverter.db");
#endif
    settings.setValue(QStringLiteral("mode"), "simple_report"); //multi_report | auto_report

    //settings.setValue(QStringLiteral("logfile_name"), "cmdb.log");
    //settings.setValue(QStringLiteral("log_level"), "info");
    settings.endGroup();
}
