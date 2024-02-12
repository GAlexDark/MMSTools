#ifndef CDATASOURCELIST_H
#define CDATASOURCELIST_H

#include <QStringList>

class CDataSourceList
{
public:
    static QStringList getDataSourceList(const QString &path, const QStringList &mask);
};

#endif // CDATASOURCELIST_H
