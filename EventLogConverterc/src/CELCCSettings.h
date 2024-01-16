/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CELCCSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for initializing the settings class instance of the
*     Event Log Conversion Console Utility.
*
****************************************************************************/

#ifndef CELCCSETTINGS_H
#define CELCCSETTINGS_H
#include "CELCSettings.h"

class CELCCSettings : public CELCSettings
{
public:
    static CELCCSettings& instance();

private:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCCSETTINGS_H
