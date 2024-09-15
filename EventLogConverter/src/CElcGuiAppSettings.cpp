/****************************************************************************
*
* The class for initializing the settings class instance and
*   creating additional settings for the MMS Event Log Conversion GUI Utility.
* Copyright (C) 2023-2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#include "CElcGuiAppSettings.h"
#include <QVariant>
#include <QDateTime>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    inline const QVariant nullStringValue = QVariant(QMetaType::fromType<QString>()); // Qt 6 only
    inline const QVariant nullDateTimeValue = QVariant(QMetaType::fromType<QDateTime>());
#else
    const QVariant nullStringValue = QVariant(QVariant::String);
    const QVariant nullDateTimeValue = QVariant(QVariant::DateTime);
#endif

CElcGuiAppSettings& CElcGuiAppSettings::instance()
{
    static CElcGuiAppSettings g_elcGuiSettings;
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
