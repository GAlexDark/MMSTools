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

void
CELCCSettings::createDefault(const QString& iniPath)
{
    CELCSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("SETTINGS"));
    settings.setValue(QStringLiteral("data_has_headers"), QStringLiteral("yes"));
    settings.endGroup();
}
