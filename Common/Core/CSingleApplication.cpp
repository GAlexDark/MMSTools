#include "CSingleApplication.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QByteArray>

#include "Debug.h"

const QString sharedMemory_prefix = "_m";
const QString semaphore_prefix = "_s";
const QString ELC_PID_FILENAME = QStringLiteral("PID");

/*
https://stackoverflow.com/questions/17431205/qsharedmemory-signals-and-qbuffer
http://berenger.eu/blog/c-qt-singleapplication-single-app-instance/
*/

CSingleApplication::CSingleApplication(const QString &id): m_id(id), m_semaphore(nullptr), m_sharedMemory(nullptr)
{
#ifdef __MSG_EXCHANGE
    m_timer = nullptr;
#endif
    m_semaphore = new QSystemSemaphore(m_id + semaphore_prefix, 1);
    Q_CHECK_PTR(m_semaphore);
}

CSingleApplication::~CSingleApplication()
{
#ifdef __MSG_EXCHANGE
    if (m_timer != nullptr) {
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        delete m_timer;
        m_timer = nullptr;
    }
#endif
    if (m_sharedMemory != nullptr) {
        if (m_sharedMemory->isAttached()) {
            m_sharedMemory->detach();
        }
        delete m_sharedMemory;
        m_sharedMemory = nullptr;
    }

    if (m_semaphore != nullptr) {
        delete m_semaphore;
        m_semaphore = nullptr;
    }
}

bool
CSingleApplication::isRunning()
{
    // true - the slave process
    // false - the master process

    bool retVal = true;
    if (!m_semaphore->acquire()) {
        __DEBUG ( QString("CSingleApplication: Acquisition error, %1").arg(m_semaphore->errorString()));
        return retVal;
    }

#ifndef Q_OS_WIN32
    // in linux / unix shared memory is not freed when the application terminates abnormally,
    // so you need to get rid of the garbage
    {
        QSharedMemory nix_fix_shared_memory(m_id + sharedMemory_prefix);
        if(nix_fix_shared_memory.attach()){
            nix_fix_shared_memory.detach();
        }
    }
#endif

    if (m_sharedMemory == nullptr) {
        m_sharedMemory = new QSharedMemory(m_id + sharedMemory_prefix);
        Q_CHECK_PTR(m_semaphore);
    }

    if (m_sharedMemory->attach()) {
        retVal = true;
    } else {
#ifdef QT_DEBUG
        // Analyze error
        if (m_sharedMemory->error() != QSharedMemory::NotFound) {
            __DEBUG( QString("CSingleApplication: Attach error, %1").arg(m_sharedMemory->errorString()) )
        } else {
            __DEBUG( QString("CSingleApplication: Create shared memory segment") )
        }
#endif
        // Create shared memory segment
        if (!m_sharedMemory->create(segSize)) {
            __DEBUG( QString("CSingleApplication: create error, %1").arg(m_sharedMemory->errorString()) )
            retVal = true;
        }
    } // m_sharedMemory->attach()

    if (!m_semaphore->release()) {
        qDebug() << "QPaySingleApplicatio∟n: release error, " << m_semaphore->errorString();
        retVal = true;
    }

    return retVal;
}

#ifdef __MSG_EXCHANGE
void
CSingleApplication::startCheckingMessage()
{
    if (m_timer == nullptr) {
        m_timer = new QTimer();
        Q_CHECK_PTR(m_timer);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(checkForMessage()));
        m_timer->start(200);
    }
}

bool
CSingleApplication::sendMessage(const QString &message)
{
    if (!isRunning()) {
        return false;
    }

    QByteArray byteArray;
    byteArray.append(char(message.size()));
    byteArray.append(message.toUtf8());
    byteArray.append('\0');

    if (byteArray.size() > segSize)
        return false;

    m_sharedMemory->lock();
    char *to = static_cast<char*> (m_sharedMemory->data());
    while(*to != '\0'){
        int sizeToRead = int(*to);
        to += sizeToRead + 1;
    }

    const char *from = byteArray.data();
    memcpy(to, from, qMin(m_sharedMemory->size(), byteArray.size()));
    m_sharedMemory->unlock();

    return true;
}

void
CSingleApplication::checkForMessage()
{   
    QStringList arguments;

    m_sharedMemory->lock();
    char *from = static_cast<char*> (m_sharedMemory->data());

    while(*from != '\0'){
        int sizeToRead = int(*from);
        ++from;

        QByteArray byteArray = QByteArray(from, sizeToRead);
        byteArray[sizeToRead] = '\0';
        from += sizeToRead;

        arguments << QString::fromUtf8(byteArray.constData());
    }

    *(char*)m_sharedMemory->data() = '\0';
    m_sharedMemory->unlock();

    if(arguments.size())
        emit messageAvailable( arguments );
}
#endif

qint64
CSingleApplication::applicationPid()
{
    return QCoreApplication::applicationPid();
}
