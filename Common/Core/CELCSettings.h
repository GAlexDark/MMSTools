/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CELCSettings.n
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for creating default settings in the ini file.
*
****************************************************************************/

#ifndef CELCSETTINGS_H
#define CELCSETTINGS_H
#include "CBasicSettings.h"

class CELCSettings : public CBasicSettings
{
protected:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCSETTINGS_H
