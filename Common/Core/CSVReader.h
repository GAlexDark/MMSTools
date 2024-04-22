/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSVLoader.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The classes for loading data from the text files to the DB.
*
****************************************************************************/

#ifndef CSVREADER_H
#define CSVREADER_H

#include <QFile>
#include <QMap>
#include <QThread>

#include "CBasicParser.h"
#include "CBasicDatabase.h"
#include "MMSTypes.h"

const qint64 defMaxFileSize = 1024 * 1024; // 1M

class CTextFileReader
{
public:
    explicit CTextFileReader();
    virtual ~CTextFileReader();

    void setFileName(const QStringList &fileNames) { m_fileNames = fileNames; }
    bool init(bool dataHasHeaders, const mms::ffs_t &ffs);
    bool read();
    QString errorString() const { return m_errorString; }

private:
    QFile       m_file;

    char        m_delimiterChar;
    char        m_quoteChar;
    QByteArray  m_eolChars;

    QByteArray  *m_buffer;
    QString     m_fileName;
    bool        m_isHeaders;

    bool checkBOM();
    bool readLargeFile();
    bool readSmallFile();

    //std::function<void(const QString&)> m_callbackFunction;

protected:
    qint64 indexOfEol(const qint64 startPos, const qint64 size);
    virtual bool convertData(const QString &line) = 0;

    QString     m_errorString;
    QStringList m_fileNames;
    quint64     m_lineNumber;

    void setFileName(const QString &fileName) { m_fileName = fileName; }
};

//-------------------------------------------------------------------------

class CMmsLogsReader: public CTextFileReader
{
public:
    CMmsLogsReader();
    ~CMmsLogsReader();
    bool init(const quint16 logId, const QString &dbFileName, bool dataHasHeaders,
              const QString &internalIpFirstOctet, mms::pragmaList_t *pragmaList);

    bool convertData(const QString &line) override;
    QString insertString() const { return m_parser->insertString(); }

private:
    pBasicParser    m_parser; // don't use the 'detele' operator, the ParserManager manage resources
    dataItem_t      m_data;
    bool initDB(const QString &dbFileName, mms::pragmaList_t *pragmaList);

protected:
    CBasicDatabase  m_db;
};

//-------------------------------------------------------------------------
class CMmsLogsThreadReader: public QThread, public CMmsLogsReader
{
    Q_OBJECT
public:
    explicit CMmsLogsThreadReader(QObject *parent = nullptr);
    void run();
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    bool m_retVal;

};

#endif // CSVREADER_H
