/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CElcConsoleAppSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance of the
*     Event Log Conversion Console Utility.
*
****************************************************************************/

#include "CElcConsoleAppSettings.h"

static CElcConsoleAppSettings g_elcConsoleSettings;

CElcConsoleAppSettings& CElcConsoleAppSettings::instance()
{
    return g_elcConsoleSettings;
}

void
CElcConsoleAppSettings::createDefault(const QString& iniPath)
{
    CElcCommonSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QLatin1String("SETTINGS"));
    settings.setValue(QLatin1String("data_has_headers"), QLatin1String("yes"));
    settings.endGroup();
}
