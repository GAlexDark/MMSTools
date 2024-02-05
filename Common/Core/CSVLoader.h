#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <QFile>
#include <QMap>
#include <QThread>

#include "CEventLogParser.h"
#include "CBasicDatabase.h"

const qint64 defBufferSize = 128 * 1024; // 128KB
const qint64 defMaxFileSize = 1024 * 1024; // 1M

using pragmaList_t = QMap<QString, QString>;

class CTextFileLoader
{
public:
    explicit CTextFileLoader();
    virtual ~CTextFileLoader();

    void setFileName(const QStringList &fileNames) { m_fileNames = fileNames; }
    bool init(bool dataHasHeaders, const QByteArray &eolChars = "\n", qint64 bufferSize = defBufferSize);
    bool read();
    QString errorString() const { return m_errorString; }

private:
    QFile       m_file;

    QByteArray  m_eolChars;
    QByteArray  *m_buffer;
    qint64      m_bufferSize;
    QString     m_fileName;
    bool        m_isHeaders;

    bool readLargeFile();
    bool readSmallFile();

    //std::function<void(const QString&)> m_callbackFunction;

protected:
    virtual void prepareData(QString &data) = 0;
    virtual bool convertData(const QString &line) =0;

    QString     m_errorString;
    QStringList m_fileNames;
    quint64      m_lineNumber;

    void setFileName(const QString &fileName) { m_fileName = fileName; }
};

//-------------------------------------------------------------------------

class CEventLogLoader: public CTextFileLoader
{
public:
    ~CEventLogLoader();
    bool init(const QString &dbFileName, bool dataHasHeaders, const QString &internalIpFirstOctet, pragmaList_t *pragmaList,
              const QByteArray &eolChars = "\n", qint64 bufferSize = defBufferSize);

    bool convertData(const QString &line) override;
    void prepareData(QString &data) override;

private:
    CEventLogParser   m_parser;

    TDataItem   m_data;
    QString     m_username,
                m_timestampISO8601,
                m_requestID,
                m_type,
                m_details,
                m_username1,
                m_authType,
                m_externalIP,
                m_internalIP;
    QDateTime   m_timestampTZ;

    bool initDB(const QString &dbFileName, pragmaList_t *pragmaList);

protected:
    CBasicDatabase  m_db;
};

//-------------------------------------------------------------------------
class CEventLogThreadLoader: public QThread, public CEventLogLoader
{
    Q_OBJECT
public:
    explicit CEventLogThreadLoader();
    void run();
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    bool m_retVal;

};

#endif // CSVLOADER_H
