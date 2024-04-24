/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: elcUtils.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The common constants and functions
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
#include <QPointer>

#include "CBasicDatabase.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

const int defaultStorageBlockSize = 32768;
const QRegularExpression envVar(QLatin1String("\\$([A-Za-z0-9_]+)"));
const QChar comma = QLatin1Char(',');
const QChar dotComma = QLatin1Char(';');

bool
elcUtils::sanitizeValue(const QString &value)
{
    return elcUtils::sanitizeValue(QLatin1String("^([a-zA-Z0-9_]+)$"), value);
}

bool
elcUtils::sanitizeValue(const QString &pattern, const QString &value)
{
    bool retVal = false;
    QRegularExpression mask(pattern);
    QRegularExpressionMatch match = mask.match(value);
    if (match.hasMatch()) {
        retVal = true;
    }
    return retVal;
}

QString
elcUtils::sanitizeValue(const QString &value, const QStringList &allowedValues, const QString &defaultValue)
{
    QString retVal;
    if (value.isEmpty()) {
        retVal = defaultValue;
    } else {
        retVal = allowedValues.contains(value, Qt::CaseInsensitive) ? value : defaultValue;
    }
    return retVal;
}

QStringList
elcUtils::parseValuesList(QString data)
{
    QStringList retVal;
    data = data.trimmed();
    data.replace(QLatin1String(" "), "");
    if ((data.indexOf(comma) != -1) || (data.indexOf(dotComma) != -1)) {
        if (data.indexOf(comma) != -1) {
            data.replace(dotComma, comma);
            retVal.append(data.split(comma));
        }
        if (data.indexOf(dotComma) != -1) {
            data.replace(comma, dotComma);
            retVal.append(data.split(dotComma));
        }
        retVal.removeAll(QString(""));
        retVal.removeDuplicates();
    } else {
        if (!data.isEmpty()) {
            retVal.append(data.trimmed());
        }
    }
    return retVal;
}

void
elcUtils::parseValuesList(QStringList &data)
{
    if (!data.isEmpty()) {
        QString buf;
        if (data.size() > 1) {
            buf = data.join(dotComma);
        } else {
            buf = data.at(0).trimmed();
        }
        if ((buf.indexOf(comma) != -1) || (buf.indexOf(dotComma) != -1)) {
            data.clear();
            data.append(parseValuesList(buf));
        }
    }
}

bool
elcUtils::trunvateDB(const QString &connectionString, QString &errorString,
                          qsizetype tablesCount, const QStringList &tablesNames,
                          const QStringList &creationStrings)
{
    CBasicDatabase db;
    bool retVal = db.init(QLatin1String("QSQLITE"), connectionString);
    if (retVal) {
        retVal = db.open();
        if (retVal) {
            for (qsizetype i = 0; i < tablesCount; ++i) {
                retVal = db.truncateTable(tablesNames.at(i));
                if (retVal) {
                    retVal = db.exec(creationStrings.at(i));
                    if (!retVal) {
                        break;
                    }
                }
            }
        }
        db.close();
    }

    if (!retVal) {
        errorString = db.errorString();
    }
    return retVal;
}

int
elcUtils::getStorageBlockSize(const QString &file)
{
    QStorageInfo storage(QDir(file).absolutePath());
    int blockSize = -1;
    if (storage.isValid()) {
        blockSize = storage.blockSize();
    }
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
    LPWSTR buf = nullptr;
    QString retVal;
    quint32 messageFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                           FORMAT_MESSAGE_FROM_SYSTEM |
                           FORMAT_MESSAGE_IGNORE_INSERTS;
    quint32 len = FormatMessage(messageFlags,
                                NULL,
                                errorCode,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                reinterpret_cast<LPWSTR>(&buf),
                                0, NULL);
    if (len > 0) {
        retVal = QString::fromWCharArray(buf);
        LocalFree(buf);
    } else {
        retVal = QStringLiteral("Unknown Windows API error: %1").arg(errorCode);
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
        wchar_t *fullPath = new wchar_t [bufSize];
        Q_CHECK_PTR(fullPath);

        bufSize = ExpandEnvironmentStrings(path.toStdWString().c_str(), fullPath, bufSize);
        if (bufSize != 0) {
            path = QString::fromWCharArray(fullPath).trimmed();
            path = QDir::fromNativeSeparators(path);
        }
        delete[] fullPath;
        fullPath = nullptr;
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
        word = '$' + match1.captured(1);
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
elcUtils::mkPath(const QString &dirPath, QString &errorString)
{
    QFileInfo fi(dirPath);
    bool retVal=fi.permission(QFile::WriteUser);
    if (retVal) {
        QDir dir;
        retVal = dir.exists(dirPath);
        if (!retVal) {
            retVal = dir.mkpath(dirPath);
            if (!retVal) {
                errorString = QStringLiteral("Error create directory path");
            }
        }
    } else {
        errorString = QStringLiteral("Access denied");
    }
    return retVal;
}

QStringList
elcUtils::getDataSourceList(const QString &path, const QStringList &mask)
{
    QStringList retVal;

    QDirIterator it(path, mask, QDir::Files);
    while(it.hasNext()) {
        retVal.append(it.next());
    }
    return retVal;
}

