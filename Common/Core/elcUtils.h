#ifndef ELCUTILS_H
#define ELCUTILS_H

#include <QString>
#include <QStringList>

namespace elcUtils
{
    bool sanitizeValue(const QString &value);
    QStringList parseValuesList(QString data);
    void parseValuesList(QStringList &data);
    bool trunvateDB(const QString &connectionString, QString &errorString);
    int getStorageBlockSize(const QString &file);

//    void expand_environment_variables(QString &path);
}
#endif // ELCUTILS_H
