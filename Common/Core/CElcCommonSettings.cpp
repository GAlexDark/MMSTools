/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CElcCommonSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for creating default settings in the ini file.
*
****************************************************************************/

#include "CElcCommonSettings.h"
#include <QFileInfo>

void
CElcCommonSettings::createDefault(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QLatin1String("SETTINGS"));
#ifdef QT_DEBUG
    QString dbName = QStringLiteral(TEST_SRCDIR"EventLogConverter.db");
#else
    QString filePath = QFileInfo(iniPath).absolutePath();
    QString fileName = QFileInfo(iniPath).baseName();
#ifdef Q_OS_WIN
    QString dbName = isRdsEnabled() ? QStringLiteral("%1/%2.db").arg(filePath, fileName) : QStringLiteral("%1.db").arg(fileName);
#else
    QString dbName = QStringLiteral("$HOME/.local/share/%1/%1.db").arg(fileName);
#endif
#endif
    settings.setValue(QLatin1String("db_file_name"), dbName);
    settings.setValue(QLatin1String("clear_on_startup"), QLatin1String("yes")); // yes | no
    settings.setValue(QLatin1String("internal_ip_start_octet"), QLatin1String("10."));
    settings.endGroup();

    settings.beginGroup(QLatin1String("DATABASE"));
    settings.setValue(QLatin1String("journal_mode"), QLatin1String("MEMORY")); // DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
    settings.setValue(QLatin1String("synchronous"), QLatin1String("NORMAL")); // OFF | NORMAL | FULL
    settings.setValue(QLatin1String("temp_store"), QLatin1String("MEMORY")); // DEFAULT | FILE | MEMORY
    settings.setValue(QLatin1String("locking_mode"), QLatin1String("EXCLUSIVE")); // NORMAL | EXCLUSIVE
    settings.endGroup();
}
