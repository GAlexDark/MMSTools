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

#ifndef ELCUTILS_H
#define ELCUTILS_H

#include <QString>
#include <QStringList>
#include <QThread>
#include <QMetaClassInfo>
#include <QFile>

namespace elcUtils
{
    bool sanitizeValue(const QString &pattern, const QString &value);
    QString sanitizeValue(const QString &value, const QStringList &allowedValues, const QString &defaultValue);
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
    QString getFileErrorMessage(const QFile::FileError error);

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
