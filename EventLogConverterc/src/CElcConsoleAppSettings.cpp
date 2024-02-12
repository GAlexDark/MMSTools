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
    settings.beginGroup(QStringLiteral("SETTINGS"));
    settings.setValue(QStringLiteral("data_has_headers"), QStringLiteral("yes"));
    settings.endGroup();
}
