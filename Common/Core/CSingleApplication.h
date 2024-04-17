#ifndef CSINGLEAPPLICATION_H
#define CSINGLEAPPLICATION_H

#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QString>

const int segSize = 128;

class CSingleApplication
{
public:
    explicit CSingleApplication(const QString &id);
    ~CSingleApplication();
    static qint64 applicationPid();
    bool isRunning();

private:
    QString              m_id;
    QSystemSemaphore    *m_semaphore;
    QSharedMemory       *m_sharedMemory;
};

#endif // CSingleApplication_H
