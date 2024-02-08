/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023
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
    QString filePath = QFileInfo(iniPath).absolutePath();
    QString fileName = QFileInfo(iniPath).baseName();

    settings.beginGroup(SettingsGroup);
#ifdef QT_DEBUG
    settings.setValue(KeyDbFileName, QStringLiteral(TEST_SRCDIR"EventLogConverter.db"));
#else
#ifdef Q_OS_WIN
    QString dbName = (m_isRdsEnabled)? QStringLiteral("%1/%2.db").arg(filePath, fileName) : QStringLiteral("%1.db").arg(fileName);
#else
    QString dbName = QStringLiteral("$HOME/.local/share/%1/%1.db").arg(fileName);
#endif
    settings.setValue(KeyDbFileName, dbName);
#endif
    settings.setValue(QStringLiteral("clear_on_startup"), QStringLiteral("yes")); // yes | no
    settings.setValue(QStringLiteral("internal_ip_start_octet"), QStringLiteral("10."));
    settings.endGroup();

    settings.beginGroup(DatabaseGroup);
    settings.setValue(QStringLiteral("journal_mode"), QStringLiteral("MEMORY")); // DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
    settings.setValue(QStringLiteral("synchronous"), QStringLiteral("NORMAL")); // OFF | NORMAL | FULL
    settings.setValue(QStringLiteral("temp_store"), QStringLiteral("MEMORY")); // DEFAULT | FILE | MEMORY
    settings.setValue(QStringLiteral("locking_mode"), QStringLiteral("EXCLUSIVE")); // NORMAL | EXCLUSIVE
    settings.endGroup();
}
