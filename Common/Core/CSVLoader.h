#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <QFile>
#include <QThread>

#include "CSVParser.h"
#include "CBasicDBClass.h"

const qint64 defBufferSize = 128 * 1024; // 128KB
const qint64 defMaxFileSize = 1024 * 1024; // 1M

class CSVLoader
{
public:
    CSVLoader();
    ~CSVLoader();

    void setFileName(const QStringList &fileNames) { m_fileNames = fileNames; }
    bool init(const QString &dbFileName, bool dataHasHeaders, const QByteArray &eolChars = "\n", qint64 bufferSize = defBufferSize);
    bool read();
    QString errorString() const { return m_errorString; }

private:
    QFile       m_file;
    QString     m_errorString;
    QByteArray  m_eolChars;
    QByteArray  *m_buffer;
    qint64      m_bufferSize;
    QString     m_fileName;
    bool        m_isHeaders;
    CBasicDBClass  m_db;
    TDataItem   m_data;

    bool initDB(const QString &dbFileName);
    bool initBuffer();

    bool readLargeFile();
    bool readSmallFile();

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

    //std::function<void(const QString&)> m_callbackFunction;

protected:
    QStringList m_fileNames;
    CSVParser   m_parser;

    void setFileName(const QString &fileName) { m_fileName = fileName; }
    bool beginTrancaction() { return m_db.beginTransaction(); }
    bool commit() { return m_db.commitTransaction(); }
    bool prepareRequest(const QString &query);
    void closeDB() { m_db.close(); }
};

//-------------------------------------------------------------------------

class CSVThreadLoader: public QThread, public CSVLoader
{
public:
    explicit CSVThreadLoader();
    void run();
    bool getStatus() const { return m_retVal; }

private:
    QString m_errorString;
    bool m_retVal;
};

#endif // CSVLOADER_H
