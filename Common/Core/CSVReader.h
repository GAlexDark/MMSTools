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
    //bool init(bool dataHasHeaders, const mms::ffs_t &ffs);
    bool read();
    QString errorString() const { return m_errorString; }

private:
    QFile       m_file;

    char        m_delimiterChar = 0;
    char        m_quoteChar = 0;
    qint64      m_eolCharsCount = 0;

    QScopedPointer<QByteArray> m_buffer;
    QString     m_fileName;
    bool        m_isHeaders = false;

    bool checkBOM();
    bool readColumnNames(const qint64 bytesRead, bool &isEOF, qint64 &prevPosition);
    bool readLargeFile();
    bool readSmallFile();

protected:
    bool init(bool dataHasHeaders, const mms::ffs_t &ffs);
    qint64 indexOfEol(const qint64 startPos, const qint64 size);
    virtual bool checkHeader(const QString &line) = 0;
    virtual bool convertData(const QString &line) = 0;
    void setFileName(const QString &fileName) { m_fileName = fileName; }

    QString     m_errorString;
    QStringList m_fileNames;
    quint64     m_lineNumber = 0;

};

//-------------------------------------------------------------------------

class CMmsLogsReader: public CTextFileReader
{
public:
    ~CMmsLogsReader();
    bool init(const quint16 logId, const QString &dbFileName, bool dataHasHeaders,
              const QString &internalIpFirstOctet, const mms::pragmaList_t *pragmaList);

    bool checkHeader(const QString &line) override;
    bool convertData(const QString &line) override;
    QString insertString() const { return m_parser->insertString(); }

private:
    pBasicParser    m_parser = nullptr; // don't use the 'detele' operator, the ParserManager manage resources
    dataItem_t      m_data;
    bool initDB(const QString &dbFileName, const mms::pragmaList_t *pragmaList);

protected:
    CBasicDatabase  m_db;
};

//-------------------------------------------------------------------------
class CMmsLogsThreadReader: public QThread, public CMmsLogsReader
{
    Q_OBJECT
public:
    explicit CMmsLogsThreadReader(QObject *parent = nullptr);
    void run() override;
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    bool m_retVal = false;

};

#endif // CSVREADER_H
