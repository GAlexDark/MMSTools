/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: CElcGuiAppSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for creating additional settings in the ini file for
*     the Event Log Conversion GUI Utility.
*
****************************************************************************/

#include "CElcGuiAppSettings.h"
#include "elcUtils.h"

static CElcGuiAppSettings g_elcGuiSettings;

CElcGuiAppSettings& CElcGuiAppSettings::instance()
{
    return g_elcGuiSettings;
}

void
CElcGuiAppSettings::createDefault(const QString& iniPath)
{
    CElcCommonSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("HISTORY"));
    settings.setValue(QStringLiteral("last_dir"), nullStringValue);
    settings.endGroup();
}
