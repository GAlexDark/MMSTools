/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CElcCommonSettings.n
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for creating default settings in the ini file.
*
****************************************************************************/

#ifndef CELCCOMMONSETTINGS_H
#define CELCCOMMONSETTINGS_H
#include "CBasicSettings.h"

class CElcCommonSettings : public CBasicSettings
{
protected:
    void createDefault(const QString& iniPath) override;
};

#endif // CELCCOMMONSETTINGS_H
