/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: elcUtils.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The common constants and functions
*
****************************************************************************/

#ifndef ELCUTILS_H
#define ELCUTILS_H

#include <QString>
#include <QStringList>
#include <QThread>
#include <QMetaClassInfo>

namespace elcUtils
{
/*
    inline const QString pvOff("OFF");
    inline const QString pvNormal("NORMAL");
    inline const QString pvFull("FULL");
    inline const QString pvDelete("DELETE");
    inline const QString pvTruncate("TRUNCATE");
    inline const QString pvPersist("PERSIST");
    inline const QString pvMemory("MEMORY");
    inline const QString pvWal("WAL");
    inline const QString pvDefault("DEFAULT");
    inline const QString pvFile("FILE");
    inline const QString pvExclusive("EXCLUSIVE");

    inline const QStringList plSynchronous = { pvOff, pvNormal, pvFull };
    inline const QStringList plJournalMode = { pvDelete, pvTruncate, pvPersist, pvMemory, pvWal, pvOff };
    inline const QStringList plTempStore = { pvDefault, pvFile, pvMemory };
    inline const QStringList plLockingMode = { pvNormal, pvExclusive };
*/
/*!
 * bool elcUtils::sanitizeValue(const QString &value).
 * Checks the \a value for invalid characters.
 * Only a-z, A-Z, 0-9, and '_' characters are allowed.
 * If the \a value contains only these characters the function returns \a true and \a false otherwise
 */

    bool sanitizeValue(const QString &value);
    bool sanitizeValue(const QString &pattern, const QString &value);
    QString sanitizeValue(const QString &value, const QStringList &allowedValues, const QString &defaultValue);
    QStringList parseValuesList(QString data);
    void parseValuesList(QStringList &data);
    bool trunvateDB(const QString &connectionString, QString &errorString,
                    qsizetype tablesCount, const QStringList &tablesNames,
                    const QStringList &creationStrings);
    int getStorageBlockSize(const QString &file);
    QString getFormattedDateTime(const QString &dateTime);
    void waitForEndThread(QThread *obj, unsigned long time);
#ifdef Q_OS_WIN
    QString getWindowsApiErrorMessage(quint32 errorCode);
    bool isRdpMode(quint32 &errorCode);
    bool isRdsMode(quint32 &errorCode);
#endif
    void expandEnvironmentStrings(QString &path);
    bool mkPath(const QString &dirPath, QString &errorString);
    QStringList getDataSourceList(const QString &path, const QStringList &mask);

    template <typename T>
    bool getMetaClassInfo(T *object, const QString &name, QString &value)
    {
        bool retVal = false;
        const QMetaObject* mObj = object->metaObject();
        for (int j = mObj->classInfoOffset(); j < mObj->classInfoCount(); j++) {
            QMetaClassInfo classInfo = mObj->classInfo(j);
            if (QString::compare(classInfo.name(), name, Qt::CaseInsensitive) == 0) {
                value = classInfo.value();
                retVal = true;
                break;
            }
        }
        return retVal;
    }
}
#endif // ELCUTILS_H
