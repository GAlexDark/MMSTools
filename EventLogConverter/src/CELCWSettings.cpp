/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: CELCSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for creating additional settings in the ini file for
*     the Event Log Conversion GUI Utility.
*
****************************************************************************/

#include "CELCWSettings.h"

static ELCWSettings g_elcwSettings;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVariant nullStringValue = QVariant(QMetaType::fromType<QString>()); // Qt 6 only
#else
    QVariant nullStringValue = QVariant(QString());
#endif

ELCWSettings& ELCWSettings::instance()
{
    return g_elcwSettings;
}

void
ELCWSettings::createDefault(const QString& iniPath)
{
    CELCSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("HISTORY"));
    settings.setValue(QStringLiteral("last_dir"), nullStringValue);
    settings.endGroup();
}
