/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: CElcGuiAppSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for creating additional settings in the ini file for
*     the Event Log Conversion GUI Utility.
*
****************************************************************************/

#include "CElcGuiAppSettings.h"
#include <QVariant>
#include <QDateTime>

static CElcGuiAppSettings g_elcGuiSettings;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    inline const QVariant nullStringValue = QVariant(QMetaType::fromType<QString>()); // Qt 6 only
    inline const QVariant nullDateTimeValue = QVariant(QMetaType::fromType<QDateTime>());
#else
    const QVariant nullStringValue = QVariant(QVariant::String);
    const QVariant nullDateTimeValue = QVariant(QVariant::DateTime);
#endif

CElcGuiAppSettings& CElcGuiAppSettings::instance()
{
    return g_elcGuiSettings;
}

void
CElcGuiAppSettings::createDefault(const QString& iniPath)
{
    CElcCommonSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QLatin1String("HISTORY"));
    settings.setValue(QLatin1String("last_dir"), nullStringValue);
    settings.endGroup();
#ifdef Q_OS_WIN
    settings.beginGroup(QLatin1String("SETTINGS"));
#ifdef QT_DEBUG
    settings.setValue(QLatin1String("default_monitor"), 1); // "\\.\DISPLAY1" in the my hardware config
#else
    settings.setValue(QLatin1String("default_monitor"), 0); // 0-auto, 1-first, 2-second, etc.
#endif
    settings.endGroup();
#endif
}

QString
CElcGuiAppSettings::getLastDir() const
{
    return getMain(QLatin1String("HISTORY/last_dir")).toString().trimmed();
}
#ifdef Q_OS_WIN
qsizetype
CElcGuiAppSettings::getDefaultMonitor() const
{
    return getMain(QLatin1String("SETTINGS/default_monitor")).toLongLong();
}
#endif
