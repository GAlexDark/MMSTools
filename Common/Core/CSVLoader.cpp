#include "CSVLoader.h"
#include <QTextStream>

#include "Debug.h"
#include "DBStrings.h"
#include "elcUtils.h"

bool
CTextFileLoader::readSmallFile()
{
    QString buffer;
    bool retVal = true;
    QTextStream textStream(&m_file);
    try {
        buffer = textStream.readAll();
        if (buffer.isEmpty()) {
            retVal = false;
        }
    } catch (...) {
        retVal = false;
        m_errorString = QStringLiteral("Error reading file");
    }
    m_file.close();

    if (retVal) {
        prepareData(buffer);

        QStringList list;
        list.append(buffer.split(m_eolChars));

        QString line;
        m_lineNumber = 1;
        qsizetype firstDataColumn = (m_isHeaders)? 1 : 0;
        qsizetype columnCount = list.size();
        for (qsizetype i = firstDataColumn; i < columnCount; ++i) {
            line = list.at(i);
            retVal = convertData(line);
            m_lineNumber++;
            if (!retVal) {
                break;
            }
            line.clear();
        }
    }

    return retVal;
}

bool
CTextFileLoader::readLargeFile()
{
    QString line;
    qint64 bufferPosition = 0;
    qint64 prevPosition = 0;
    qint64 nextPosition;
    qint64 eolCharsLen = m_eolChars.length();
    bool isEOF = false;
    bool retVal = true;
    qint64 bytesRead;
    m_lineNumber = 1;

    memset(m_buffer->data(), 0, m_buffer->size());

    m_file.seek(bufferPosition);
    bytesRead = m_file.read(m_buffer->data(), m_bufferSize);
    if (bytesRead > 0) {
        //If data has header
        if (m_isHeaders) {
            nextPosition = m_buffer->indexOf(m_eolChars, prevPosition);
            if (nextPosition != -1) {
                line.clear();
                line.append(m_buffer->mid(prevPosition, nextPosition));
                m_lineNumber++;

                prevPosition = nextPosition + eolCharsLen;
            }
        }
    } else {
        isEOF = true;
    }

    while(!isEOF && retVal) {
        while (!isEOF && retVal) {
            nextPosition = m_buffer->indexOf(m_eolChars, prevPosition);
            if (nextPosition == -1) {
                if (bytesRead < m_bufferSize) {
                    isEOF = true;
                    nextPosition = bytesRead;
                } else {
                    break;
                }
            }
            line.clear();
            line.append(m_buffer->mid(prevPosition, nextPosition - prevPosition));
            retVal = convertData(line);
            m_lineNumber++;

            prevPosition = nextPosition + eolCharsLen;
        } // internal while

        if (!isEOF) {
            bufferPosition = bufferPosition + prevPosition;
            m_file.seek(bufferPosition);
            memset(m_buffer->data(), 0, m_buffer->size());
            prevPosition = 0;
            bytesRead = m_file.read(m_buffer->data(), m_bufferSize);
            if (bytesRead <= 0) {
                isEOF = true;
            }
        }
    } // external while
    m_file.close();
    return retVal;
}

CTextFileLoader::CTextFileLoader()
    : m_buffer(nullptr), m_bufferSize(defBufferSize), m_fileName(""), m_isHeaders(false), m_errorString(""), m_lineNumber(0)
{
    m_eolChars.clear();
    m_fileNames.clear();
}

CTextFileLoader::~CTextFileLoader()
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
CTextFileLoader::init(bool dataHasHeaders,
                     const QByteArray &eolChars, qint64 bufferSize)
{
    m_isHeaders = dataHasHeaders;
    m_eolChars = eolChars;
    m_bufferSize = bufferSize;

    bool retVal = true;
    try {
        if (m_bufferSize > 0) {
            m_buffer = new QByteArray(m_bufferSize, 0);

        } else {
            retVal = false;
            m_errorString = QStringLiteral("Invalid buffer size");
        }
    } catch (const std::bad_alloc &e) {
        m_errorString = e.what();
        retVal = false;
    }
    return retVal;
}

bool
CTextFileLoader::read()
{
    m_file.setFileName(m_fileName);
    qint64 size = m_file.size();
#ifdef QT_DEBUG
  #ifdef TEST_LARGE
    size = defMaxFileSize + 1;
  #endif
#endif
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (retVal) {
        retVal = (size < defMaxFileSize)? readSmallFile() : readLargeFile();
    } else {
        m_errorString = m_file.errorString();
    }
    return retVal;
}

//--------------------------------------------------------------------------

bool
CEventLogLoader::initDB(const QString &dbFileName, pragmaList_t *pragmaList)
{
    bool retVal = m_db.init("QSQLITE", dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            QStringList dbCommandItems;
            dbCommandItems.append(pragmaUTF8);
            dbCommandItems.append(pragmaSynchronous.arg(pragmaList->value("synchronous")));

            int blockSize = elcUtils::getStorageBlockSize(dbFileName);
            dbCommandItems.append(pragmaPageSize.arg(blockSize));

            dbCommandItems.append(pragmaJournalMode.arg(pragmaList->value("journal_mode")));
            dbCommandItems.append(pragmaTempStore.arg(pragmaList->value("temp_store")));
            dbCommandItems.append(pragmaLockingMode.arg(pragmaList->value("locking_mode")));
            dbCommandItems.append(createEventLogTable);

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
        TDataList res = m_db.findInDB("PRAGMA journal_mode;", false);
        QString value = "journal_mode: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
        res = m_db.findInDB("PRAGMA page_size;", false);
        value = "page_size: " + QString::fromStdWString(res.at(0).at(0).toStdWString());
        __DEBUG( value )
        res.clear();
        res = m_db.findInDB("PRAGMA cache_size;", false);
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

CEventLogLoader::~CEventLogLoader()
{
    m_db.close();
    m_data.clear();
}

bool
CEventLogLoader::init(const QString &dbFileName, bool dataHasHeaders, const QString &internalIpFirstOctet, pragmaList_t *pragmaList,
          const QByteArray &eolChars, qint64 bufferSize)
{
    m_parser.init(internalIpFirstOctet);

    bool retVal = CTextFileLoader::init(dataHasHeaders, eolChars, bufferSize);
    if (retVal) {
        retVal = initDB(dbFileName, pragmaList);
    }

    return retVal;
}

bool CEventLogLoader::convertData(const QString &line)
{
    bool retVal = m_parser.parse(line);
    if (retVal) {
        m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                               m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

        m_data[QStringLiteral(":username")] = m_username;
        m_data[QStringLiteral(":timestampISO8601")] = m_timestampISO8601;
        m_data[QStringLiteral(":timestamp")] = m_timestampTZ;
        m_data[QStringLiteral(":requestid")] = m_requestID;
        m_data[QStringLiteral(":type")] = m_type;
        m_data[QStringLiteral(":details")] = m_details;
        m_data[QStringLiteral(":username1")] = m_username1;
        m_data[QStringLiteral(":authtype")] = m_authType;
        m_data[QStringLiteral(":externalip")] = m_externalIP;
        m_data[QStringLiteral(":internalip")] = m_internalIP;
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

void
CEventLogLoader::prepareData(QString &data)
{
    data.replace("\r\n", "@RN@", Qt::CaseInsensitive );
    data.replace("\n", "@N@", Qt::CaseInsensitive);
    data.replace("@RN@", "\r\n", Qt::CaseInsensitive );
}

//--------------------------------------------------------------------------

CEventLogThreadLoader::CEventLogThreadLoader()
    : m_retVal(false)
{}

void
CEventLogThreadLoader::run()
{
    m_errorString.clear();
    //-----------------------------------------------------
    if (m_fileNames.size() > 0 ) {
        emit sendMessage( tr("Preparing to read the file(s).") );
        m_retVal = m_db.beginTransaction();
        if (m_retVal) {
            m_retVal = m_db.prepareRequest(insertOriginalData);
            if (m_retVal) {
                QString fileName;
                qsizetype filesCount = m_fileNames.size();
                for (qsizetype i = 0; i < filesCount; ++i) {
                    fileName = m_fileNames.at(i);
                    emit sendMessage( tr("Reading of the file %1 has started.").arg(fileName) );
                    setFileName(fileName);
                    m_retVal = read();
                    if (m_retVal) {
                        m_retVal = m_db.commitTransaction();
                        if (!m_retVal) {
                            m_errorString = m_db.errorString();
                            break;
                        }
                    }
                    QString msg = (m_retVal)? tr("The file %1 was read").arg(fileName) : tr("The file %1 was not read").arg(fileName);
                    emit sendMessage( msg );
                } //for
            }
        }
        m_db.close();
    } // m_fileNames.size()
}
