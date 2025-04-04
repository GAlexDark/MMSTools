/****************************************************************************
*
* The classes for loading data from the text files to the Database.
* Copyright (C) 2023-2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CSVREADER_H
#define CSVREADER_H

#include <QFile>
#include <QMap>
#include <QThread>

#include "CBasicParser.h"
#include "CSqliteDatabase.h"
#include "MMSTypes.h"

const qint64 defMaxFileSize = 1024 * 1024; // 1M

class CTextFileReader
{
public:
    explicit CTextFileReader();
    virtual ~CTextFileReader();
    bool read();
    const QString& errorString() const { return m_errorString; }

private:
    QFile       m_file;

    char        m_delimiterChar = 0;
    char        m_quoteChar = 0;
    qint64      m_eolCharsCount = 0;

    QScopedPointer<QByteArray> m_buffer;
    QString     m_fileName;
    bool        m_isHeaders = false;
    QString     m_errorString;
    quint64     m_lineNumber = 0;

    bool checkBOM();
    bool readColumnNames(const qint64 bytesRead, bool &isEOF, qint64 &prevPosition);
    bool startRead(qint64 &bytesRead, qint64 &prevPosition, bool &isEof);
    bool readLargeFile();
    bool readSmallFile();

protected:
    bool init(bool dataHasHeaders, const mms::ffs_t &ffs);
    qint64 indexOfEol(const qint64 startPos, const qint64 size) const;
    virtual bool checkHeader(const QString &line) = 0;
    virtual bool convertData(const QString &line) = 0;
    void setFileName(const QString &fileName) { m_fileName = fileName; }
    void setErrorString(const QString &errorString) { m_errorString = errorString; }
    void clearErrorString() { m_errorString.clear(); }
    quint64 getLineNumber() const { return m_lineNumber; }
};

//-------------------------------------------------------------------------

class CMmsLogsReader: public CTextFileReader
{
public:
    ~CMmsLogsReader() override;
    bool init(const quint16 logId, const QString &dbFileName, bool dataHasHeaders,
              const QString &internalIpFirstOctet, const QMap<QString, QString> *pragmaList);

    bool checkHeader(const QString &line) override;
    bool convertData(const QString &line) override;
    QString insertString() const { return m_parser->insertString(); }

private:
    pBasicParser    m_parser = nullptr; // don't use the 'detele' operator, the ParserManager manage resources
    bool initDB(const QString &dbFileName, const QMap<QString, QString> *pragmaList);

protected:
    CSqliteDatabase  m_db;
};

//-------------------------------------------------------------------------
class CMmsLogsThreadReader: public QThread, public CMmsLogsReader
{
    Q_OBJECT
public:
    explicit CMmsLogsThreadReader(QObject *parent = nullptr);
    void setFileNames(const QStringList &fileNames) { m_fileNames = fileNames; }
    void run() override;
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    bool m_retVal = false;
    QStringList m_fileNames;
};

class MmsLogsReaderError : public mms::MmsCommonException
{
public:
    explicit MmsLogsReaderError(const QString &text) noexcept
        : MmsCommonException(text) {}
};

#endif // CSVREADER_H
