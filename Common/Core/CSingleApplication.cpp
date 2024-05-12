#include "CSingleApplication.h"
#include <QCoreApplication>
#ifdef QT_DEBUG
#include "Debug.h"
#endif

const QString sharedMemory_prefix = QLatin1String("_m");
const QString semaphore_prefix = QLatin1String("_s");
const QString ELC_PID_FILENAME = QLatin1String("PID");

/*
https://stackoverflow.com/questions/17431205/qsharedmemory-signals-and-qbuffer
http://berenger.eu/blog/c-qt-singleapplication-single-app-instance/
*/

CSingleApplication::CSingleApplication(const QString &id) : m_id(id) {
  m_semaphore.reset(new QSystemSemaphore(m_id + semaphore_prefix, 1));
  Q_CHECK_PTR(m_semaphore);
}

CSingleApplication::~CSingleApplication() {
  if (m_sharedMemory && m_sharedMemory->isAttached()) {
    m_sharedMemory->detach();
  }
}

qint64 CSingleApplication::applicationPid() {
  return QCoreApplication::applicationPid();
}

bool CSingleApplication::isRunning() {
  // true - slave process
  // false - master process

  if (!m_semaphore->acquire()) {
#ifdef QT_DEBUG
    __DEBUG(QStringLiteral("CSingleApplication: Acquisition error, %1")
                .arg(m_semaphore->errorString()))
#endif
    return true;
  }
  bool isRun = false;
#ifndef Q_OS_WIN32
  // In the linux / unix shared memory is not freed when the application
  // terminates abnormally, so you need to get rid of the garbage
  {
    QSharedMemory nix_fix_shared_memory(m_id + sharedMemory_prefix);
    if (nix_fix_shared_memory.attach()) {
      nix_fix_shared_memory.detach();
    }
  }
#endif

  if (!m_sharedMemory) {
    m_sharedMemory.reset(new QSharedMemory(m_id + sharedMemory_prefix));
    Q_CHECK_PTR(m_sharedMemory);
  }

  if (m_sharedMemory->attach()) {
    isRun = true;
  } else {
    isRun = false;

#ifdef QT_DEBUG
    // Analyze error
    if (m_sharedMemory->error() != QSharedMemory::NotFound) {
      __DEBUG(QStringLiteral("CSingleApplication: Attach error, %1")
                  .arg(m_sharedMemory->errorString()))
    } else {
      __DEBUG(
          QStringLiteral("CSingleApplication: Create shared memory segment"))
    }
#endif
    // Create shared memory segment
    if (!m_sharedMemory->create(segSize)) {
      isRun = true;
    }
  }

  if (!m_semaphore->release()) {
#ifdef QT_DEBUG
    __DEBUG(QStringLiteral("CSingleApplication: release error, %1")
                .arg(m_semaphore->errorString()))
#endif
    return true;
  }

  return isRun;
}
