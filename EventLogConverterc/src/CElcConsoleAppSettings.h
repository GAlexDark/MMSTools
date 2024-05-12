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

class CElcConsoleAppSettings : public CElcCommonSettings {
public:
  static CElcConsoleAppSettings &instance();
  bool isDataHasHeaders() const;

private:
  void createDefault(const QString &iniPath) override;
};

#endif // CELCCONSOLEAPPSETTINGS_H
