#include "elcUtils.h"
#include "qthread.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStorageInfo>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

#include "CBasicDatabase.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

const int defaultStorageBlockSize = 32768;

bool
elcUtils::sanitizeValue(const QString &value)
{
    bool retVal = false;
    static QRegularExpression mask("^([a-zA-Z0-9_]+)$");
    QRegularExpressionMatch match = mask.match(value);
    if (match.hasMatch()) {
        retVal = true;
    }
    return retVal;
}

QStringList
elcUtils::parseValuesList(QString data)
{
    QStringList retVal;
    data = data.trimmed();
    data.replace(" ", "");
    if ((data.indexOf(',') != -1) || (data.indexOf(';') != -1)) {
        if (data.indexOf(',') != -1) {
            data.replace(';', ',');
            retVal.append(data.split(','));
        }
        if (data.indexOf(';') != -1) {
            data.replace(',', ';');
            retVal.append(data.split(';'));
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
        if (data.size() == 1) {
            QString buf = data.at(0).trimmed();
            if ((buf.indexOf(',') != -1) || (buf.indexOf(';') != -1)) {
                data.clear();
                data.append(parseValuesList(buf));
            }
        } else {
            data.removeAll(QString(""));
            data.removeDuplicates();
        }
    }
}

bool
elcUtils::trunvateDB(const QString &connectionString, QString &errorString)
{
    CBasicDatabase db;
    bool retVal = db.init("QSQLITE", connectionString);
    if (retVal) {
        retVal = db.open();
        if (retVal) {
            retVal = db.truncateTable("eventlog");
            db.close();
        }
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
    return (blockSize != -1)? blockSize : defaultStorageBlockSize;
}

QString
elcUtils::getFormattedDateTime(const QString &dateTime)
{
    QDateTime buildDate = QDateTime::fromString(dateTime);
    return buildDate.toString("yyyyMMddhhmm");
}

void
elcUtils::waitForEndThread(QThread *obj, unsigned long time)
{
    while (!obj->wait(time)) {
        QCoreApplication::processEvents();
    }
}

#ifdef Q_OS_WIN
bool
elcUtils::isRemoteSessionMode(quint32 &errorCode)
{
    SetLastError(ERROR_SUCCESS);
    int retVal = GetSystemMetrics(SM_REMOTESESSION);
    errorCode = GetLastError();
    return (retVal > 0)? true : false;
}

bool
elcUtils::isTerminalServerMode(quint32 &errorCode)
{
    SetLastError(ERROR_SUCCESS);
    OSVERSIONINFOEX osinfo;
    ZeroMemory(&osinfo, sizeof(OSVERSIONINFOEX));
    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osinfo.wSuiteMask = VER_SUITE_TERMINAL;

    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_SUITENAME, VER_AND);

    bool isTSM = false;
    if (VerifyVersionInfo(&osinfo, VER_SUITENAME, conditionMask)) {
        errorCode = ERROR_SUCCESS;
        if (osinfo.wSuiteMask & VER_SUITE_TERMINAL) {
            isTSM = true;
        }
    } else {
        errorCode = GetLastError();
        if (errorCode == ERROR_OLD_WIN_VERSION) {
            errorCode = ERROR_SUCCESS;
        }
    }
    return isTSM;
}

void
expandEnvStrings_windows(QString &path)
{
    DWORD bufSize = ExpandEnvironmentStrings(path.toStdWString().c_str(), NULL, 0);
    if (bufSize != 0) {
        wchar_t *fullPath = new wchar_t [bufSize];
        Q_CHECK_PTR(fullPath);

        if (ExpandEnvironmentStrings(path.toStdWString().c_str(), fullPath, bufSize) != 0) {
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
    QRegularExpression envVar("\\$([A-Za-z0-9_]+)");
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

