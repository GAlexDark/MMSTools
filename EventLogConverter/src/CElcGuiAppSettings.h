/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: CElcGuiAppSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance and
*     creating additional settings for the Event Log Conversion GUI Utility.
*
****************************************************************************/

#ifndef CELCGUIAPPSETTINGS_H
#define CELCGUIAPPSETTINGS_H

#include <CElcCommonSettings.h>

inline const QString HistoryLastDir = QStringLiteral("HISTORY/last_dir");
inline const QString KeyLastDir = QStringLiteral("last_dir");
inline const QString HistoryGroup = QStringLiteral("HISTORY");

class CElcGuiAppSettings : public CElcCommonSettings
{
public:
    static CElcGuiAppSettings& instance();

private:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCGUIAPPSETTINGS_H
