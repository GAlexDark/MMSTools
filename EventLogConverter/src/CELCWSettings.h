/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: CELCWSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance and
*     creating additional settings for the Event Log Conversion GUI Utility.
*
****************************************************************************/

#ifndef CELCWSETTINGS_H
#define CELCWSETTINGS_H

#include <CELCSettings.h>

class ELCWSettings : public CELCSettings
{
public:
    static ELCWSettings& instance();

private:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCWSETTINGS_H
