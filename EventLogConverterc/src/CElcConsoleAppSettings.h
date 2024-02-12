/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CElcConsoleAppSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance of the
*     Event Log Conversion Console Utility.
*
****************************************************************************/

#ifndef CELCCONSOLEAPPSETTINGS_H
#define CELCCONSOLEAPPSETTINGS_H
#include "CElcCommonSettings.h"

inline const QString SettingsDataHasHeaders = QStringLiteral("SETTINGS/data_has_headers");

class CElcConsoleAppSettings : public CElcCommonSettings
{
public:
    static CElcConsoleAppSettings& instance();

private:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCCONSOLEAPPSETTINGS_H
