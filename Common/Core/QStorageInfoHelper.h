#ifndef QSTORAGEINFOHELPER_H
#define QSTORAGEINFOHELPER_H

#include <QStorageInfo>



class QStorageInfoHelper
{
public:
    explicit QStorageInfoHelper() {}
    static int getStorageBlockSize(const QString &file)
    {
        QStorageInfo storage(QDir(file).absolutePath());
        int blockSize = -1;
        if (storage.isValid()) {
            blockSize = storage.blockSize();
        }
        return (blockSize != -1)? blockSize : 32768;
    }
};


#endif // QSTORAGEINFOHELPER_H
