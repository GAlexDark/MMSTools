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

#include <QMetaClassInfo>
#include <QString>
#include <QStringList>
#include <QThread>

namespace elcUtils {
bool sanitizeValue(const QString &pattern, const QString &value);
QString sanitizeValue(const QString &value, const QStringList &allowedValues,
                      const QString &defaultValue);
QStringList parseValuesList(QString data);
void parseValuesList(QStringList &data);
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
bool getMetaClassInfo(T *object, const QString &name, QString &value) {
  bool retVal = false;
  const QMetaObject *mObj = object->metaObject();
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
} // namespace elcUtils
#endif // ELCUTILS_H
