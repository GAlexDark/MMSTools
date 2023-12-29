#ifndef CSINGLEAPPLICATION_H
#define CSINGLEAPPLICATION_H

#include <QSystemSemaphore>
#include <QSharedMemory>
#ifdef __MSG_EXCHANGE
  #include <QTimer>
#endif
#include <QString>
#include <QObject>

const int segSize = 128;

class CSingleApplication : public QObject
{
    Q_OBJECT
public:
    explicit CSingleApplication(const QString &id);
    ~CSingleApplication();

    bool isRunning();

#ifdef __MSG_EXCHANGE
/**
 * @brief Инициализация обмена данными через QSharedMemory
 */
    void startCheckingMessage();

/**
 * @brief Отправка сообщения через QSharedMemory
 * @param message
 * @return true если отправка успешная, false если размер данных в байтах больше segSize
 */
    bool sendMessage(const QString &message);
#endif

    static qint64 applicationPid();

#ifdef __MSG_EXCHANGE
private slots:
        void checkForMessage();

signals:
/**
 * @brief cигнал, который вырабатывается при получении данных через QSharedMemory
 * @param message
 */
    void messageAvailable(const QStringList& messages);
#endif

private:
    QString              m_id;
    QSystemSemaphore    *m_semaphore;
    QSharedMemory       *m_sharedMemory;
#ifdef __MSG_EXCHANGE
    QTimer              *m_timer;
#endif
};

#endif // CSingleApplication_H
