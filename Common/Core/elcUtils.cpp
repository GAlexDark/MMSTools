#include "elcUtils.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStorageInfo>
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN
    #include "windows.h"
#endif

#include "CBasicDatabase.h"

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

#ifdef Q_OS_WIN
void
ExpandEnvStrings(QString &path)
{
    QString buf = QDir::fromNativeSeparators(path);
    DWORD bufSize = ExpandEnvironmentStrings(buf.toStdWString().c_str(), NULL, 0);
    if (bufSize != 0) {
        wchar_t *fullPath = new wchar_t [bufSize];
        Q_CHECK_PTR(fullPath);

        if (ExpandEnvironmentStrings(buf.toStdWString().c_str(), fullPath, bufSize) != 0) {
            buf = QString::fromWCharArray(fullPath).trimmed();
            path = QDir::fromNativeSeparators(buf);
        }
        delete[] fullPath;
        fullPath = nullptr;
    }
}
#endif


void
expand_environment_variables(QString &path)
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
        word.clear();
        value.clear();
    }
}

