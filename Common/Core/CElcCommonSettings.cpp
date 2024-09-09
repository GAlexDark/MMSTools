/****************************************************************************
*
* The Event Log Conversion Utility base class for creating default settings
*   in the ini file.
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

#include "CElcCommonSettings.h"
#include <QFileInfo>
#include <QDir>
#include "elcUtils.h"

const QString pvOff("OFF");
const QString pvNormal("NORMAL");
const QString pvFull("FULL");
const QString pvDelete("DELETE");
const QString pvTruncate("TRUNCATE");
const QString pvPersist("PERSIST");
const QString pvMemory("MEMORY");
const QString pvWal("WAL");
const QString pvDefault("DEFAULT");
const QString pvFile("FILE");
const QString pvExclusive("EXCLUSIVE");
const QString defaultAllowedChars("a-zA-Z0-9_");

const QStringList plSynchronous = { pvOff, pvNormal, pvFull };
const QStringList plJournalMode = { pvDelete, pvTruncate, pvPersist, pvMemory, pvWal, pvOff };
const QStringList plTempStore = { pvDefault, pvFile, pvMemory };
const QStringList plLockingMode = { pvNormal, pvExclusive };

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
    settings.setValue(QLatin1String("journal_mode"), pvMemory); // DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
    settings.setValue(QLatin1String("synchronous"), pvNormal); // OFF | NORMAL | FULL
    settings.setValue(QLatin1String("temp_store"), pvMemory); // DEFAULT | FILE | MEMORY
    settings.setValue(QLatin1String("locking_mode"), pvExclusive); // NORMAL | EXCLUSIVE
    settings.endGroup();

    settings.beginGroup(QLatin1String("REPORT"));
    settings.setValue(QLatin1String("show_milliseconds"), QLatin1String("no")); // yes | no
    settings.endGroup();

    settings.beginGroup(QLatin1String("BASIC_FILTER"));
    settings.setValue(QLatin1String("allowed_chars"), defaultAllowedChars);
    settings.endGroup();
}

QString
CElcCommonSettings::getDbFileName() const
{
    return QDir::fromNativeSeparators(getMain(QLatin1String("SETTINGS/db_file_name")).toString().trimmed());
}

bool
CElcCommonSettings::isClearDbOnStartup() const
{
    QString buf = getMain(QLatin1String("SETTINGS/clear_on_startup")).toString().trimmed();
    return buf.isEmpty() || (QString::compare(buf, QLatin1String("yes"), Qt::CaseInsensitive) == 0);
}

QString
CElcCommonSettings::getInternalIpStartOctet() const
{
    QString buf = getMain(QLatin1String("SETTINGS/internal_ip_start_octet")).toString().trimmed();
    return !buf.isEmpty() && elcUtils::sanitizeValue(QLatin1String("^([0-9.]+)$"), buf) ? buf : QString();
}

QString
CElcCommonSettings::getSynchronousType() const
{
    QString buf = getMain(QLatin1String("DATABASE/synchronous")).toString().trimmed();
    return elcUtils::sanitizeValue(buf, plSynchronous, pvNormal);
}

QString
CElcCommonSettings::getJournalModeType() const
{
    QString buf = getMain(QLatin1String("DATABASE/journal_mode")).toString().trimmed();
    return elcUtils::sanitizeValue(buf, plJournalMode, pvMemory);
}

QString
CElcCommonSettings::getTempStore() const
{
    QString buf = getMain(QLatin1String("DATABASE/temp_store")).toString().trimmed();
    return elcUtils::sanitizeValue(buf, plTempStore, pvMemory);
}

QString
CElcCommonSettings::getLockingMode() const
{
    QString buf = getMain(QLatin1String("DATABASE/locking_mode")).toString().trimmed();
    return elcUtils::sanitizeValue(buf, plLockingMode, pvExclusive);
}

bool
CElcCommonSettings::getShowMilliseconds() const
{
    QString buf = getMain(QLatin1String("REPORT/show_milliseconds")).toString().trimmed();
    return buf.isEmpty() || (QString::compare(buf, QLatin1String("no"), Qt::CaseInsensitive) == 0) ? false : true;
}

QString
CElcCommonSettings::getAllowedChars() const
{
    QString buf = getMain(QLatin1String("BASIC_FILTER/allowed_chars")).toString().trimmed();
    return buf.isEmpty() ? defaultAllowedChars : buf;
}
