#ifndef ELCUTILS_H
#define ELCUTILS_H

#include <QString>
#include <QStringList>
#include <QThread>

namespace elcUtils
{
    bool sanitizeValue(const QString &value);
    QStringList parseValuesList(QString data);
    void parseValuesList(QStringList &data);
    bool trunvateDB(const QString &connectionString, QString &errorString);
    int getStorageBlockSize(const QString &file);
    QString getFormattedDateTime(const QString &dateTime);
    void waitForEndThread(QThread *obj, unsigned long time);
#ifdef Q_OS_WIN
    bool isRemoteSessionMode(quint32 &errorCode);
    bool isTerminalServerMode(quint32 &errorCode);
#endif
    void expandEnvironmentStrings(QString &path);
}
#endif // ELCUTILS_H
