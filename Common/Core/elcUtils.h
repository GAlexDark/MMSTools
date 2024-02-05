#ifndef ELCUTILS_H
#define ELCUTILS_H

#include <QString>
#include <QStringList>
#include <QThread>
#include <QVariant>
#include <QDateTime>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    inline const QVariant nullStringValue = QVariant(QMetaType::fromType<QString>()); // Qt 6 only
    inline const QVariant nullDateTimeValue = QVariant(QMetaType::fromType<QDateTime>());
#else
    const QVariant nullStringValue = QVariant(QVariant::String);
    const QVariant nullDateTimeValue = QVariant(QVariant::DateTime);
#endif

namespace elcUtils
{
/*!
 * bool elcUtils::sanitizeValue(const QString &value).
 * Checks the \a value for invalid characters.
 * Only a-z, A-Z, 0-9, and '_' characters are allowed.
 * If the \a value contains only these characters the function returns \a true and \a false otherwise
 */
    bool sanitizeValue(const QString &value);
    bool sanitizeValue(const QString &pattern, const QString &value);
    QString sanitizeValue(const QString &value, const QStringList &alloedValues, const QString &defaultValue);
    QStringList parseValuesList(QString data);
    void parseValuesList(QStringList &data);
    bool trunvateDB(const QString &connectionString, QString &errorString);
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
}
#endif // ELCUTILS_H
