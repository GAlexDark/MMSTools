/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSVLoader.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The classes for loading data from the text files to the DB.
*
****************************************************************************/

#include "CSVReader.h"
#include <QElapsedTimer>

#ifdef QT_DEBUG
  #include "Debug.h"
#endif
#include "DBStrings.h"
#include "elcUtils.h"
#include "CParserManager.h"

bool
CTextFileReader::checkBOM(const QByteArray &buffer)
{
    bool retVal = true;
    m_errorString.clear();
    static const char utf8bomRaw[] = {'\xef', '\xbb', '\xbf'};
    QByteArray utf8bom = QByteArray::fromRawData(utf8bomRaw, sizeof(utf8bomRaw));
    QByteArray bom = buffer.sliced(0, sizeof(utf8bomRaw));
    if (bom != utf8bom) {
        retVal = false;
        m_errorString = QStringLiteral("Wrong BOM header. The file mush have UTF-8 BOM 0xef 0xbb 0xbf header");
    }

    return retVal;
}

qint64
CTextFileReader::indexOfEol(const QByteArray *data, const qint64 startPos, const qint64 size)
{
    qint64 retVal = -1;
    qint64 index = startPos;
    qint64 eolCharLength = m_eolChars.length();
    bool isQuoted = false;
    bool isFound = false;

    while (!isFound && (index <= size - eolCharLength)) {
        if (data->at(index) == m_quoteChar) {
            isQuoted = !isQuoted;
            index++;
        } else {
            switch (eolCharLength) {
            case 1:
                if (data->at(index) == '\n') {
                    if (isQuoted) {
                        index++;
                    } else {
                        retVal = index;
                        isFound = true;
                    }
                } else {
                    index++;
                }
                break;
            case 2:
                if (data->at(index) == '\r') {
                    if (data->at(index + 1) == '\n') {
                        if (isQuoted) {
                            index++;
                        } else {
                            retVal = index;
                            isFound = true;
                        }
                    }
                } else {
                    index++;
                }
                break;
            }
        } // else
    } //while

    return retVal;
}

bool
CTextFileReader::readSmallFile()
{
    // "284 590 milliseconds" 4,74316667 minutes
    // "317 529 milliseconds" 5,29215 minutes
    // "292 992 milliseconds" 4,8832 minutes
    // "132 515 milliseconds" 2,20858333 minutes
    // "139 280 milliseconds" 2,32133333 minutes
    // "139 681 milliseconds" 2,32801667 minutes
    // "141 998 milliseconds" 2.3666 minutes
    // "143 329 milliseconds"
    // "132 359 milliseconds"
    // "134 293 milliseconds" 2.2382 minutes
    // "134 005 milliseconds"

/*
 * If you know that pos and len cannot be out of bounds,
 * use sliced() instead in new code, because it is faster.
 */

    bool retVal = true;
    qint64 bytesRead;
    try {
        bytesRead = m_file.read(m_buffer->data(), m_file.size());
    } catch (...) {
        retVal = false;
        m_errorString = QStringLiteral("Error reading file");
    }
    m_file.close();

    if (bytesRead > 0) {
        retVal = checkBOM(*m_buffer);
        if (retVal) {
            QString line;
            bool isEOF = false;
            qint64 prevPosition = 3;
            qint64 nextPosition;
            qint64 eolCharsLen = m_eolChars.length();
            m_lineNumber = 0;

            //If data has header
            if (m_isHeaders) {
                nextPosition = indexOfEol(m_buffer, prevPosition, bytesRead);
                if (nextPosition != -1) {
                    line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
                    prevPosition = nextPosition + eolCharsLen;
                    m_lineNumber++;
                } else {
                    isEOF = true;
                }
            }

            while (!isEOF && retVal) {
                nextPosition = indexOfEol(m_buffer, prevPosition, bytesRead);
                line.clear();
                if (nextPosition != -1) {
                    line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
                    retVal = convertData(line);
                    prevPosition = nextPosition + eolCharsLen;
                } else {
                    if (prevPosition < bytesRead) {
                        // The line don't have the EOL
                        line = m_buffer->sliced(prevPosition, bytesRead - prevPosition);
                        retVal = convertData(line);
                    }
                    // prevPosition == bufferSize - The line is empty or/and the EOF
                    isEOF = true;
                }
                m_lineNumber++;
            } // internal while
        }
    }

    return retVal;
}

bool
CTextFileReader::readLargeFile()
{
    // "288 154 milliseconds" 4.8026 minutes
    // "137 051 milliseconds" 2.2842 minutes
    // "133 530 milliseconds" 2.2255 minutes
    // "137 403 milliseconds"
    // "126 503 milliseconds" 2.1084 minutes
    QString line;
    qint64 bufferOffset = 0;
    qint64 eolCharsLen = m_eolChars.length();
    bool isEOF = false;
    bool retVal = true;
    m_lineNumber = 0;

    m_file.seek(bufferOffset);

    qint64 bytesRead = m_file.read(m_buffer->data(), defMaxFileSize);
    if (bytesRead > 0) {
        qint64 prevPosition = 3;
        qint64 nextPosition;
        retVal = checkBOM(*m_buffer);
        if (retVal && m_isHeaders) {
            nextPosition = indexOfEol(m_buffer, prevPosition, defMaxFileSize);
            if (nextPosition != -1) {
                line.clear();
                line = m_buffer->sliced(prevPosition, nextPosition - prevPosition); // prevPosition == 0
                prevPosition = nextPosition + eolCharsLen;
                m_lineNumber++;
            } else {
                isEOF = true;
            }
        } //m_isHeaders

        while (!isEOF && retVal) {
            do {
                nextPosition = indexOfEol(m_buffer, prevPosition, defMaxFileSize);
                if (nextPosition != -1) {
                    line.clear();
                    line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
                    prevPosition = nextPosition + eolCharsLen;
                    retVal = convertData(line);
                    m_lineNumber++;
                }
            } while ((nextPosition != -1) && retVal);

            if (bytesRead == defMaxFileSize) {
                bufferOffset += prevPosition;
                prevPosition = 0;
                m_file.seek(bufferOffset);
                memset(m_buffer->data(), 0, m_buffer->size());
                bytesRead = m_file.read(m_buffer->data(), defMaxFileSize);
                if (bytesRead <= 0) {
                    isEOF = true;
                }
            } else {
                //This is last record in the file
                if (prevPosition != bytesRead) {
                    line.clear();
                    line = m_buffer->sliced(prevPosition, bytesRead - prevPosition);
                    retVal = convertData(line);
                    m_lineNumber++;
                }
                isEOF = true;
            }
        } //while
    } //bytesRead > 0

    m_file.close();
    return retVal;
}

CTextFileReader::CTextFileReader()
    : m_delimiterChar(0), m_quoteChar(0), m_buffer(nullptr), m_fileName(""), m_isHeaders(false), m_errorString(""), m_lineNumber(0)
{
    m_eolChars.clear();
    m_fileNames.clear();
}

CTextFileReader::~CTextFileReader()
{
    m_eolChars.clear();

    delete m_buffer;
    m_buffer = nullptr;

    if (m_file.isOpen()) {
        m_file.close();
    }

    m_fileNames.clear();
}

bool
CTextFileReader::init(bool dataHasHeaders, const mms::ffs_t &ffs)
{
    m_isHeaders = dataHasHeaders;
    m_delimiterChar = ffs.delimiterChar;
    m_quoteChar = ffs.quoteChar;
    m_eolChars = ffs.eolChars;

    bool retVal = true;
    delete m_buffer;

    try {
        m_buffer = new QByteArray(defMaxFileSize, 0);
        Q_CHECK_PTR(m_buffer);
    } catch (const std::bad_alloc &e) {
        m_errorString = e.what();
        retVal = false;
    }

    return retVal;
}

bool
CTextFileReader::read()
{
    m_file.setFileName(m_fileName);
    qint64 size = m_file.size();
#ifdef QT_DEBUG
  #ifdef TEST_LARGE
    size = defMaxFileSize + 1;
  #endif
#endif
    bool retVal = m_file.open(QIODevice::ReadOnly);
    memset(m_buffer->data(), 0, defMaxFileSize);
    if (retVal) {
        retVal = (size < defMaxFileSize)? readSmallFile() : readLargeFile();
        //retVal = readSmallFile();
        //retVal = readLargeFile();
    } else {
        m_errorString = m_file.errorString();
    }
    return retVal;
}

//--------------------------------------------------------------------------

bool
CMmsLogsReader::initDB(const QString &dbFileName, mms::pragmaList_t *pragmaList)
{
    bool retVal = m_db.init(QLatin1String("QSQLITE"), dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            QStringList dbCommandItems;
            dbCommandItems.append(pragmaUTF8);
            dbCommandItems.append(pragmaSynchronous.arg(pragmaList->value(QLatin1String("synchronous"))));

            int blockSize = elcUtils::getStorageBlockSize(dbFileName);
            dbCommandItems.append(pragmaPageSize.arg(blockSize));

            dbCommandItems.append(pragmaJournalMode.arg(pragmaList->value(QLatin1String("journal_mode"))));
            dbCommandItems.append(pragmaTempStore.arg(pragmaList->value(QLatin1String("temp_store"))));
            dbCommandItems.append(pragmaLockingMode.arg(pragmaList->value(QLatin1String("locking_mode"))));
            const CParserManager &parserManager = CParserManager::instance();
            dbCommandItems.append(parserManager.getCreateTableRequestList());

            for (qsizetype i = 0; i < dbCommandItems.size(); ++i) {
                retVal = m_db.exec(dbCommandItems.at(i));
                if (!retVal) {
                    break;
                }
            }
        }
    }
#ifdef QT_DEBUG
/********************************************************
 *
 * this code only for diagnostic in the debug mode
 *
 *******************************************************/
    if (retVal) {
        dataList_t res = m_db.findInDB(QLatin1String("PRAGMA journal_mode;"), false);
        QString value = "journal_mode: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
        res = m_db.findInDB(QLatin1String("PRAGMA page_size;"), false);
        value = "page_size: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
        res = m_db.findInDB(QLatin1String("PRAGMA cache_size;"), false);
        value = "cache_size: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
    }
#endif
    if (!retVal) {
        m_errorString = m_db.errorString();
        m_db.close();
    }
    return retVal;
}

CMmsLogsReader::CMmsLogsReader()
    : m_parser(nullptr)
{}

CMmsLogsReader::~CMmsLogsReader()
{
    m_db.close();
}

bool
CMmsLogsReader::init(const quint16 logId, const QString &dbFileName, bool dataHasHeaders, const QString &internalIpFirstOctet,
                     mms::pragmaList_t *pragmaList)
{
    CParserManager &parserManager = CParserManager::instance();
    m_parser = nullptr;
    bool retVal = parserManager.checkID(logId);
    if (retVal) {
        m_parser = parserManager.getInstance(logId);
        Q_CHECK_PTR(m_parser);
        m_parser->init(internalIpFirstOctet);
        retVal = CTextFileReader::init(dataHasHeaders, m_parser->fileFieldsSeparationInfo());
        if (retVal) {
            retVal = initDB(dbFileName, pragmaList);
        }
    } else {
        m_errorString = QStringLiteral("The parser not found");
    }

    return retVal;
}

bool CMmsLogsReader::convertData(const QString &line)
{
    bool retVal = m_parser->parse(line);
    if (retVal) {
        m_parser->convertData(m_data);
        retVal = m_db.execRequest(&m_data);
        m_data.clear();
        if (!retVal) {
            m_errorString = m_db.errorString();
        }
    } else {
        m_errorString = QStringLiteral("Parsing error at line number %1: %2").arg(m_lineNumber).arg(line);
    }
    return retVal;
}

//--------------------------------------------------------------------------

CMmsLogsThreadReader::CMmsLogsThreadReader()
    : m_retVal(false)
{}

void
CMmsLogsThreadReader::run()
{
    m_errorString.clear();
    QElapsedTimer timer;
    timer.start();
    //-----------------------------------------------------
    if (m_fileNames.size() > 0 ) {
        emit sendMessage( tr("Preparing to read the file(s).") );
        QString fileName;
        qsizetype filesCount = m_fileNames.size();
        for (qsizetype i = 0; i < filesCount; ++i) {
            m_retVal = m_db.beginTransaction();
            if (m_retVal) {
                m_retVal = m_db.prepareRequest(insertString());
                if (m_retVal) {
                    fileName = m_fileNames.at(i);
                    emit sendMessage( tr("Reading of the file %1 has started.").arg(fileName) );
                    setFileName(fileName);
                    m_retVal = read();
                    if (m_retVal) {
                        m_retVal = m_db.commitTransaction();
                        QString msg = (m_retVal)? tr("The file %1 was read").arg(fileName) : tr("The file %1 was not read").arg(fileName);
                        emit sendMessage( msg );
                    }
                }
            }
            if (!m_retVal) {
                m_errorString = m_db.errorString();
                break;
            }
        } //for
    m_db.close();
    } // m_fileNames.size()
    QString msg = QStringLiteral("%1 milliseconds").arg(timer.elapsed() );
    __DEBUG( msg )
}
