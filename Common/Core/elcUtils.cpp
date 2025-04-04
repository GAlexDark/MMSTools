/****************************************************************************
*
* The common functions.
* Copyright (C) 2024  Oleksii Gaienko
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

#include "elcUtils.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStorageInfo>
#include <QDir>
#include <QDirIterator>
#include <QCoreApplication>
#include <QScopedPointer>

#ifdef Q_OS_WIN
#include "windows.h"
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

namespace {
    const int defaultStorageBlockSize = 32768;
    const QRegularExpression envVar(QLatin1String("\\$([A-Za-z0-9_]+)"));
    const QChar comma = QLatin1Char(',');
    const QChar dotComma = QLatin1Char(';');
}

bool
elcUtils::sanitizeValue(const QString &pattern, const QString &value)
{
    QRegularExpression mask(pattern);
    return mask.match(value).hasMatch();
}

QString
elcUtils::sanitizeValue(const QString &value, const QStringList &allowedValues, const QString &defaultValue)
{
    if (value.isEmpty()) {
        return defaultValue;
    } else {
        return allowedValues.contains(value, Qt::CaseInsensitive) ? value : defaultValue;
    }
}

QStringList
elcUtils::parseValuesList(QString data)
{
    QStringList retVal;
    data = data.trimmed().replace(QLatin1String(" "), "");
    if (data.contains(comma) || data.contains(dotComma)) {
        data.replace(dotComma, comma);
        retVal = data.split(comma);
        retVal.removeAll(QString());
        retVal.removeDuplicates();
    } else if (!data.isEmpty()) {
        retVal.append(data.trimmed());
    }
    return retVal;
}

void
elcUtils::parseValuesList(QStringList &data)
{
    if (!data.isEmpty()) {
        QString buf = (data.size() > 1) ? data.join(dotComma) : data.at(0).trimmed();
        if (buf.contains(comma) || buf.contains(dotComma)) {
            data.clear();
            data.append(parseValuesList(buf));
        }
    }
}

int
elcUtils::getStorageBlockSize(const QString &file)
{
    QStorageInfo storage(QDir(file).absolutePath());
    int blockSize = storage.isValid() ? storage.blockSize() : -1;
    return blockSize == -1 ? defaultStorageBlockSize : blockSize;
}

QString
elcUtils::getFormattedDateTime(const QString &dateTime)
{
    QDateTime buildDate = QDateTime::fromString(dateTime);
    return buildDate.toString(QLatin1String("yyyyMMddhhmm"));
}

void
elcUtils::waitForEndThread(QThread *obj, unsigned long time)
{
    while (!obj->wait(time)) {
        QCoreApplication::processEvents();
    }
}

#ifdef Q_OS_WIN

QString
elcUtils::getWindowsApiErrorMessage(quint32 errorCode)
{
    wchar_t *buf = nullptr;
    QString retVal(QStringLiteral("Unknown Windows API error: %1").arg(errorCode));
    const quint32 messageFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                           FORMAT_MESSAGE_FROM_SYSTEM |
                           FORMAT_MESSAGE_IGNORE_INSERTS;
    quint32 len = FormatMessage(messageFlags, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                reinterpret_cast<wchar_t*>(&buf), 0, NULL);
    if (len > 0) {
        retVal = QString::fromWCharArray(buf);
        LocalFree(buf);
    }
    return retVal;
}

bool
elcUtils::isRdpMode(quint32 &errorCode)
{
    SetLastErrorEx(ERROR_SUCCESS, 0);
    bool retVal = true;
    errorCode = ERROR_SUCCESS;
    if (GetSystemMetrics(SM_REMOTESESSION) == 0) {
        retVal = false;
        errorCode = GetLastError();
    }
    return retVal;
}

bool
elcUtils::isRdsMode(quint32 &errorCode)
{
    //ref: https://swissdelphicenter.com/en/printcode.php?id=1968
    SetLastErrorEx(ERROR_SUCCESS, 0);
    OSVERSIONINFOEX osinfo;
    ZeroMemory(&osinfo, sizeof(OSVERSIONINFOEX));
    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osinfo.wSuiteMask = VER_SUITE_TERMINAL;

    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_SUITENAME, VER_AND);

    bool isRds = false;
    if (VerifyVersionInfo(&osinfo, VER_SUITENAME, conditionMask)) {
        errorCode = ERROR_SUCCESS;
        if (osinfo.wSuiteMask & VER_SUITE_TERMINAL) {
            isRds = true;
        }
    } else {
        errorCode = GetLastError();
        if (errorCode == ERROR_OLD_WIN_VERSION) {
            errorCode = ERROR_SUCCESS;
        }
    }
    return isRds;
}

void
expandEnvStrings_windows(QString &path)
{
    SetLastErrorEx(ERROR_SUCCESS, 0);
    DWORD bufSize = ExpandEnvironmentStrings(path.toStdWString().c_str(), NULL, 0);
    if (bufSize != 0) {
        QScopedPointer<wchar_t> fullPath(new wchar_t [bufSize]);
        Q_CHECK_PTR(fullPath);
        bufSize = ExpandEnvironmentStrings(path.toStdWString().c_str(), fullPath.data(), bufSize);
        if (bufSize != 0) {
            path = QString::fromWCharArray(fullPath.data()).trimmed();
            path = QDir::fromNativeSeparators(path);
        }
    }
}
#endif

void
expandEnvStrings_linux(QString &path)
{
    QRegularExpressionMatchIterator i = envVar.globalMatch(path);
    QString word;
    QByteArray value;
    while (i.hasNext()) {
        QRegularExpressionMatch match1 = i.next();
        word = QLatin1Char('$') + match1.captured(1);
        value = qgetenv(match1.captured(1).toLatin1().data());
        path.replace(word, value, Qt::CaseInsensitive);
        path = QDir::fromNativeSeparators(path);
        word.clear();
        value.clear();
    }
}

void
elcUtils::expandEnvironmentStrings(QString &path)
{
#ifdef Q_OS_WIN
    expandEnvStrings_windows(path);
#else
    expandEnvStrings_linux(path);
#endif
}

bool
elcUtils::isFolderWritable(const QString &path)
{
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup++; // turn checking on
#endif
    bool retVal = QFileInfo(path).isWritable();
#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup--; // turn it off again
#endif
    return retVal;
}

bool
elcUtils::mkPath(const QString &dirPath, QString &errorString)
{
    QDir dir(dirPath);
    bool retVal = dir.exists();
    if (retVal) {
        retVal = isFolderWritable(dirPath);
        if (!retVal) {
            errorString = QStringLiteral("Access denied");
        }
    } else {
        QString path = QFileInfo(dirPath).absolutePath();
        retVal = isFolderWritable(path);
        if (retVal) {
            retVal = dir.mkpath(dirPath);
            if (!retVal) {
                errorString = QStringLiteral("Error create directory path");
            }
        } else {
            errorString = QStringLiteral("Access denied");
        }
    }
    return retVal;
}

QStringList
elcUtils::getDataSourceList(const QString &path, const QStringList &mask)
{
    QStringList retVal;
    QDirIterator it(path, mask, QDir::Files);
    while (it.hasNext()) {
        retVal.append(it.next());
    }
    return retVal;
}
