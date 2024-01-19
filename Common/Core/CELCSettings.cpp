/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CELCSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for creating default settings in the ini file.
*
****************************************************************************/

#include "CELCSettings.h"
#include <QCoreApplication>
void
CELCSettings::createDefault(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);

    settings.beginGroup(QStringLiteral("SETTINGS"));
#ifdef QT_DEBUG
    settings.setValue(QStringLiteral("db_file_name"), QStringLiteral(TEST_SRCDIR"EventLogConverter.db"));
#else
    QString dbName = QStringLiteral("%1.db").arg(QCoreApplication::applicationName());
    settings.setValue(QStringLiteral("db_file_name"), dbName);
#endif
    settings.setValue(QStringLiteral("clear_on_startup"), QStringLiteral("yes")); // yes | no
    settings.setValue(QStringLiteral("internal_ip_start_octet"), QStringLiteral("10."));
    settings.endGroup();

    settings.beginGroup(QStringLiteral("DATABASE"));
    settings.setValue(QStringLiteral("journal_mode"), QStringLiteral("MEMORY")); // DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
    settings.setValue(QStringLiteral("synchronous"), QStringLiteral("NORMAL")); // OFF | NORMAL | FULL
    settings.setValue(QStringLiteral("temp_store"), QStringLiteral("MEMORY")); // DEFAULT | FILE | MEMORY
    settings.setValue(QStringLiteral("locking_mode"), QStringLiteral("EXCLUSIVE")); // NORMAL | EXCLUSIVE
    settings.endGroup();
}
