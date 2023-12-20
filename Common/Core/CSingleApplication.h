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
    CSingleApplication(const QString &id);
    ~CSingleApplication();

    bool isRunning();

#ifdef __MSG_EXCHANGE
/****************************************************************************
 *
 * startCheckingMessage() - Включение обмена данными через QSharedMemory
 *
 ***************************************************************************/
    void startCheckingMessage();

/****************************************************************************
 *
 * bool sendMessage(const QString &message) - отправка сообщения через QSharedMemory
 * успешная отправка - true.
 * если размер данных в байтах больше segSize, то возвращается ошибка.
 *
 ***************************************************************************/
    bool sendMessage(const QString &message);
#endif

    static qint64 applicationPid();

#ifdef __MSG_EXCHANGE
private slots:
        void checkForMessage();

signals:
/***************************************************************************
 *
 * void messageAvailable(const QStringList& messages) - cигнал, который вырабатывается при
 * получении данных через QSharedMemory
 *
 **************************************************************************/
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
