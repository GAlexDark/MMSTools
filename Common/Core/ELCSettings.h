/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Utility
*
*  Module name: ELCSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Basic slass for read-write ini-settings
*
****************************************************************************/

#ifndef ELCSETTINGS_H
#define ELCSETTINGS_H
#include "QPayKioskSettings.h"

class ELCSettings : public QPayKioskSettings
{
public:
    static ELCSettings& instance();

//private:
protected:
    void createDefault(const QString& iniPath) override;
};

#endif // ELCSETTINGS_H
