#include "CDataSourceList.h"

#include <QDirIterator>
#include <QDebug>

QStringList CDataSourceList::getDataSourceList(const QString &path, const QStringList &mask)
{
    QStringList retVal;

    QDirIterator it(path, mask, QDir::Files);
    while(it.hasNext()) {
        retVal.append(it.next());
    }
    return retVal;
}
