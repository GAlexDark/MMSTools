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

#include "CSVReader.h"
#ifdef QT_DEBUG
  #include <QElapsedTimer>
  #include "Debug.h"
#endif
#include "DBStrings.h"
#include "elcUtils.h"
#include "CParserManager.h"

bool
CTextFileReader::checkBOM()
{
    bool retVal = true;
    m_errorString.clear();
    static const char utf8bomRaw[] = {'\xef', '\xbb', '\xbf'};
    QByteArray utf8bom = QByteArray::fromRawData(utf8bomRaw, sizeof(utf8bomRaw));
    QByteArray bom = m_buffer->sliced(0, sizeof(utf8bomRaw));
    if (bom != utf8bom) {
        retVal = false;
        m_errorString = QStringLiteral("Wrong the BOM header. The file must have the UTF-8 BOM 0xef 0xbb 0xbf header.");
    }
    return retVal;
}

qint64
CTextFileReader::indexOfEol(const qint64 startPos, const qint64 size) const
{
    qint64 retVal = -1;
    if (startPos >= 0) {
        qint64 index = startPos;
        qint64 endPos = size - m_eolCharsCount;
        bool isQuoted = false;
        bool isSecondPart = false;

        const char *d = m_buffer->constData() + index;

        while (index <= endPos) {
            if (*d == m_quoteChar) {
                isQuoted = !isQuoted;
            } else if ((*d == '\n') && !isQuoted) {
                retVal = isSecondPart ? index - 1 : index;
                break;
            } else {
                isSecondPart = (m_eolCharsCount == 2) && (*d == '\r') && !isQuoted;
            }
            ++d;
            ++index;
        }
    }
    return retVal;
}

bool
CTextFileReader::readColumnNames(const qint64 bytesRead, bool &isEOF, qint64 &prevPosition)
{
    bool retVal = true;
    qint64 nextPosition = indexOfEol(prevPosition, bytesRead);
    if (nextPosition != -1) {
        QString line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
        retVal = checkHeader(line);
        if (!retVal) {
            m_errorString = QStringLiteral("Error in the column names: %1").arg(line);
        }
        prevPosition = nextPosition + m_eolCharsCount;
        m_lineNumber++;
    } else {
        isEOF = true;
    }
    return retVal;
}
bool CTextFileReader::readSmallFile() {
    /*
   * If you know that pos and len cannot be out of bounds,
   * use sliced() instead in new code, because it is faster.
   */

    qint64 bytesRead = m_file.read(m_buffer->data(), defMaxFileSize);

    if (bytesRead <= 0) {
        m_errorString = QStringLiteral("Error reading file: %1");
        return false;
    }
    if (!checkBOM()) {
        return false;
    }
    bool isEOF = false;
    qint64 prevPosition = 3;
    if (m_isHeaders) {
        if (!readColumnNames(bytesRead, isEOF, prevPosition)) {
            return false;
        }
    }

    bool retVal = true;
    QString line;
    qint64 nextPosition;
    m_lineNumber = 0;
    while (!isEOF && retVal) {
        nextPosition = indexOfEol(prevPosition, bytesRead);
        line.clear();
        if (nextPosition != -1) {
            line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
            retVal = convertData(line);
            prevPosition = nextPosition + m_eolCharsCount;
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
    }

    return retVal;
}

bool
CTextFileReader::readLargeFile()
{
    qint64 bufferOffset = 0;
    m_file.seek(bufferOffset);

    qint64 bytesRead = m_file.read(m_buffer->data(), defMaxFileSize);
    if (bytesRead <= 0) {
        m_errorString = QStringLiteral("Error reading file: %1");
        return false;
    }
    if (!checkBOM()) {
        return false;
    }
    bool isEOF = false;
    qint64 prevPosition = 3;
    if (m_isHeaders) {
        if (!readColumnNames(bytesRead, isEOF, prevPosition)) {
            return false;
        }
    }

    bool retVal = true;
    QString line;
    qint64 nextPosition;
    m_lineNumber = 0;
    while (!isEOF && retVal) {
        do {
            nextPosition = indexOfEol(prevPosition, bytesRead);
            if (nextPosition != -1) {
                line.clear();
                line = m_buffer->sliced(prevPosition, nextPosition - prevPosition);
                prevPosition = nextPosition + m_eolCharsCount;
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
    }

    return retVal;
}

CTextFileReader::CTextFileReader()
{
    m_fileName.clear();
    m_errorString.clear();
}

CTextFileReader::~CTextFileReader()
{
    if (m_file.isOpen()) {
        m_file.close();
    }
}

bool
CTextFileReader::init(bool dataHasHeaders, const mms::ffs_t &ffs)
{
    m_isHeaders = dataHasHeaders;

    m_delimiterChar = ffs.delimiterChar;
    Q_ASSERT_X(m_delimiterChar != 0, Q_FUNC_INFO, "Wrong Delimiter char");
    m_quoteChar = ffs.quoteChar;
    QByteArray eolChars = ffs.eolChars;
    m_eolCharsCount = eolChars.length();
    Q_ASSERT_X((m_eolCharsCount == 1) || (m_eolCharsCount == 2), Q_FUNC_INFO, "Wrong EOL chars");

    m_buffer.reset(new QByteArray(defMaxFileSize, 0));

    return m_buffer ? true : false;
}

bool
CTextFileReader::read()
{
    m_file.setFileName(m_fileName);
    qint64 size = m_file.size();
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (retVal) {
        memset(m_buffer->data(), 0, defMaxFileSize);
        retVal = size <= defMaxFileSize ? readSmallFile() : readLargeFile();
        m_file.close();
    } else {
        m_errorString = m_file.errorString();
    }
    return retVal;
}

//--------------------------------------------------------------------------

bool
CMmsLogsReader::initDB(const QString &dbFileName, const QMap<QString, QString> *pragmaList)
{
    bool retVal = m_db.init(dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            QStringList dbCommandItems;
            dbCommandItems.append(pragmaUTF8);
            dbCommandItems.append(pragmaMmapSize);
            dbCommandItems.append(pragmaThreads);
            dbCommandItems.append(pragmaSynchronous.arg(pragmaList->value(QLatin1String("synchronous"))));

            int blockSize = elcUtils::getStorageBlockSize(dbFileName);
            dbCommandItems.append(pragmaPageSize.arg(blockSize));

            dbCommandItems.append(pragmaJournalMode.arg(pragmaList->value(QLatin1String("journal_mode"))));
            dbCommandItems.append(pragmaTempStore.arg(pragmaList->value(QLatin1String("temp_store"))));
            dbCommandItems.append(pragmaLockingMode.arg(pragmaList->value(QLatin1String("locking_mode"))));

            const CParserManager &parserManager = CParserManager::instance();
            dbCommandItems.append(parserManager.getCreateTableRequestList());

            qsizetype i = 0;
            while ((i < dbCommandItems.size()) && retVal) {
                retVal = m_db.exec(dbCommandItems.at(i));
                ++i;
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
        res.clear();
        res = m_db.findInDB(QLatin1String("pragma mmap_size;"), false);
        value = "mmap_size: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
        res = m_db.findInDB(QLatin1String("pragma threads;"), false);
        value = "threads: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
    }
#endif
    if (!retVal) {
        setErrorString(m_db.errorString());
        m_db.close();
    }
    return retVal;
}

CMmsLogsReader::~CMmsLogsReader()
{
    m_db.close();
}

bool
CMmsLogsReader::init(const quint16 logId, const QString &dbFileName, bool dataHasHeaders, const QString &internalIpFirstOctet,
                     const QMap<QString, QString> *pragmaList)
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
        setErrorString(QStringLiteral("The parser with the same ID is not found."));
    }

    return retVal;
}

bool
CMmsLogsReader::checkHeader(const QString &line)
{
    Q_CHECK_PTR(m_parser);
    return m_parser->checkHeader(line);
}

bool
CMmsLogsReader::convertData(const QString &line)
{
    Q_CHECK_PTR(m_parser);
    if (!m_parser->parse(line)) {
        setErrorString(QStringLiteral("Parsing error at line number %1: %2").arg(getLineNumber()).arg(line));
        return false;
    }
    QMap<QString, QVariant> data;
    m_parser->convertData(data);
    if (!m_db.execRequest(&data)) {
        setErrorString(m_db.errorString());
        return false;
    }
    return true;
}

CMmsLogsThreadReader::CMmsLogsThreadReader(QObject *parent)
    : QThread{parent}
{
    m_fileNames.clear();
}

void
CMmsLogsThreadReader::run()
{
    clearErrorString();
#ifdef QT_DEBUG
    QElapsedTimer timer;
    timer.start();
#endif
    if (m_fileNames.isEmpty() ) {
        return;
    }

    emit sendMessage( tr("Preparing to read the file(s).") );
    QString fileName;
    QString msg;
    qsizetype i = 0;
    qsizetype filesCount = m_fileNames.size();
    m_retVal = true;
    while ((i < filesCount) && m_retVal) {
        try {
            fileName = m_fileNames.at(i);
            if (!m_db.beginTransaction() || !m_db.prepareRequest(insertString())) {
                setErrorString(m_db.errorString());
                throw MmsLogsReaderError(tr("The file %1 was not read").arg(fileName));
            }
            emit sendMessage( tr("Reading of the file %1 has started.").arg(fileName) );
            setFileName(fileName);
            if (!read()) {
                throw MmsLogsReaderError(tr("The file %1 was not read").arg(fileName));
            }
            if (!m_db.commitTransaction()) {
                setErrorString(m_db.errorString());
                throw MmsLogsReaderError(tr("The file %1 was not read").arg(fileName));
            }
            msg = tr("The file %1 was read").arg(fileName);
        } catch (const MmsLogsReaderError &ex) {
            msg = ex.what();
            m_retVal = false;
        }
        emit sendMessage( msg );
        ++i;
    }
    m_db.close();
#ifdef QT_DEBUG
    msg = QStringLiteral("%1 milliseconds").arg(timer.elapsed() );
    __DEBUG( msg )
#endif
}
