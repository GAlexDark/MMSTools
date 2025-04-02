#include "CSingleApplication.h"
#include <QCoreApplication>
#ifdef QT_DEBUG
  #include "Debug.h"
#endif

const QString SHARED_MEMORY_PREFIX = QLatin1String("_m");
const QString SEMAPHORE_PREFIX = QLatin1String("_s");
const QString ELC_PID_FILENAME = QLatin1String("PID");

/*
https://stackoverflow.com/questions/17431205/qsharedmemory-signals-and-qbuffer
http://berenger.eu/blog/c-qt-singleapplication-single-app-instance/
*/

CSingleApplication::CSingleApplication(const QString &id)
    : m_id(id)
{
    m_semaphore.reset(new QSystemSemaphore(m_id + SEMAPHORE_PREFIX, 1));
    Q_CHECK_PTR(m_semaphore);
}

CSingleApplication::~CSingleApplication()
{
    if (m_sharedMemory && m_sharedMemory->isAttached()) {
        m_sharedMemory->detach();
    }
}

qint64
CSingleApplication::applicationPid()
{
    return QCoreApplication::applicationPid();
}

bool
CSingleApplication::isRunning()
{
    // true - slave process
    // false - master process

    if (!m_semaphore->acquire()) {
#ifdef QT_DEBUG
        __DEBUG( QStringLiteral("CSingleApplication: Acquisition error, %1").arg(m_semaphore->errorString()) )
#endif
        return true;
    }

    bool isRunning = false;

#ifndef Q_OS_WIN32
    // In Linux / Unix shared memory is not freed when the application terminates abnormally,
    // so you need to get rid of the garbage
    {
        QSharedMemory nixFixSharedMemory(m_id + SHARED_MEMORY_PREFIX);
        if (nixFixSharedMemory.attach()) {
            nixFixSharedMemory.detach();
        }
    }
#endif

    if (!m_sharedMemory) {
        m_sharedMemory.reset(new QSharedMemory(m_id + SHARED_MEMORY_PREFIX));
        Q_CHECK_PTR(m_sharedMemory);
    }

    if (m_sharedMemory->attach()) {
        isRunning = true;
    } else {
        isRunning = false;

#ifdef QT_DEBUG
        // Analyze error
        if (m_sharedMemory->error() != QSharedMemory::NotFound) {
            __DEBUG( QStringLiteral("CSingleApplication: Attach error, %1").arg(m_sharedMemory->errorString()) )
        } else {
            __DEBUG( QStringLiteral("CSingleApplication: Create shared memory segment") )
        }
#endif
        // Create shared memory segment
        if (!m_sharedMemory->create(segSize)) {
            isRunning = true;
        }
    }

    if (!m_semaphore->release()) {
#ifdef QT_DEBUG
        __DEBUG( QStringLiteral("CSingleApplication: release error, %1").arg(m_semaphore->errorString()) )
#endif
        return true;
    }

    return isRunning;
}
