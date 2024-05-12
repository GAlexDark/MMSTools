#ifndef CSINGLEAPPLICATION_H
#define CSINGLEAPPLICATION_H

#include <QScopedPointer>
#include <QSharedMemory>
#include <QString>
#include <QSystemSemaphore>

const int segSize = 128;

class CSingleApplication {
public:
  explicit CSingleApplication(const QString &id);
  ~CSingleApplication();
  static qint64 applicationPid();
  bool isRunning();

private:
  CSingleApplication(const CSingleApplication &) = delete;
  CSingleApplication &operator=(CSingleApplication &) = delete;

  QString m_id;
  QScopedPointer<QSystemSemaphore> m_semaphore;
  QScopedPointer<QSharedMemory> m_sharedMemory;
};

#endif // CSingleApplication_H
