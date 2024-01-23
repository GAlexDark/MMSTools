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
#include <QFileInfo>

void
CELCSettings::createDefault(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);
    QString filePath = QFileInfo(iniPath).absolutePath();
    QString fileName = QFileInfo(iniPath).baseName();

    settings.beginGroup(QStringLiteral("SETTINGS"));
#ifdef QT_DEBUG
    settings.setValue(QStringLiteral("db_file_name"), QStringLiteral(TEST_SRCDIR"EventLogConverter.db"));
#else
#ifdef Q_OS_WIN
    QString dbName = (m_isTerminalMode)? QStringLiteral("%1/%2.db").arg(filePath, fileName) : QStringLiteral("%1.db").arg(fileName);
#else
    QString dbName = QStringLiteral("%1/%2.db").arg(filePath, fileName);
#endif
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
