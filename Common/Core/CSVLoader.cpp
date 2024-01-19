#include "CSVLoader.h"
#include <QTextStream>

//#include "Debug.h"
#include "DBStrings.h"
#include "elcUtils.h"

bool
CSVLoader::initDB(const QString &dbFileName, pragmaList_t pragmaList)
{
    bool retVal = m_db.init("QSQLITE", dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            QStringList dbCommandItems;
            dbCommandItems.append(pragmaUTF8);

            QString value = pragmaList.value("synchronous", "NORMAL");
            dbCommandItems.append(pragmaSynchronous.arg(value));

            int blockSize = elcUtils::getStorageBlockSize(dbFileName);
            dbCommandItems.append(pragmaPageSize.arg(blockSize));

            value = pragmaList.value("journal_mode", "MEMORY");
            dbCommandItems.append(pragmaJournalMode.arg(value));

            value = pragmaList.value("temp_store", "MEMORY");
            dbCommandItems.append(pragmaTempStore.arg(value));

            value = pragmaList.value("locking_mode", "NORMAL");
            dbCommandItems.append(pragmaLockingMode.arg(value));

            dbCommandItems.append(createEventLogTable);

            for (qsizetype i = 0; i < dbCommandItems.size(); ++i) {
                retVal = m_db.exec(dbCommandItems.at(i));
                if (!retVal) {
                    break;
                }
            }
        }
    }
    if (!retVal) {
        m_errorString = m_db.errorString();
        m_db.close();
    }
    return retVal;
}

bool
CSVLoader::initBuffer()
{
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
CSVLoader::prepareRequest(const QString &query)
{
    return m_db.prepareRequest(query);
}

//-----------------------------------------------------------

bool
CSVLoader::readSmallFile()
{
    QString buffer;
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (retVal) {
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
    }
    m_file.close();

    if (retVal) {
        buffer.replace("\r\n", "@RN@", Qt::CaseInsensitive );
        buffer.replace("\n", "@N@", Qt::CaseInsensitive);
        buffer.replace("@RN@", "\r\n", Qt::CaseInsensitive );

        QStringList list;
        list.append(buffer.split(m_eolChars));

        QString line;
        qsizetype firstDataColumn = (m_isHeaders)? 1 : 0;
        qsizetype columnCount = list.size();
        for (qsizetype i = firstDataColumn; i < columnCount; ++i) {
            line = list.at(i);
            m_parser.parse(line);
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
            retVal = m_db.execRequest(m_data);
            if (!retVal) {
                m_errorString = m_db.errorString();
                break;
            }
            line.clear();
        }
    }

    return retVal;
}

bool
CSVLoader::readLargeFile()
{
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (retVal) {
        QString line;
        qint64 bufferPosition = 0;
        qint64 prevPosition = 0;
        qint64 nextPosition;
        qint64 eolCharsLen = m_eolChars.length();
        bool isEOF = false;
        qint64 bytesRead;

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

                m_parser.parse(line);
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
                retVal = m_db.execRequest(m_data);
                if (!retVal) {
                    //isEOF = true;
                    m_errorString = m_db.errorString();
                }

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
    } else {
        m_errorString = m_file.errorString();
    }
    return retVal;
}

CSVLoader::CSVLoader()
    : m_errorString(""), m_buffer(nullptr), m_bufferSize(defBufferSize), m_fileName(""), m_isHeaders(false)
{
    m_eolChars.clear();
    m_fileNames.clear();
}

CSVLoader::~CSVLoader()
{
    delete m_buffer;
    m_buffer = nullptr;

    if (m_file.isOpen()) {
        m_file.close();
    }
    closeDB();
}

bool
CSVLoader::init(const QString &dbFileName, bool dataHasHeaders, const QString &internalipFirstOctet, pragmaList_t pragmaList,
                     const QByteArray &eolChars, qint64 bufferSize)
{
    m_eolChars = eolChars;
    m_isHeaders = dataHasHeaders;
    m_bufferSize = bufferSize;

    m_parser.init(internalipFirstOctet);

    bool retVal = initDB(dbFileName, pragmaList);
    if (retVal) {
        retVal = initBuffer();
    }
    if (!retVal) {
        closeDB();
    }

    return retVal;
}

bool
CSVLoader::read()
{
    m_file.setFileName(m_fileName);
    qint64 size = m_file.size();
#ifdef QT_DEBUG
  #ifdef TEST_LARGE
    size = defMaxFileSize + 1;
  #endif
#endif
    return (size < defMaxFileSize)? readSmallFile() : readLargeFile();
}

//--------------------------------------------------------------------------

CSVThreadLoader::CSVThreadLoader()
    : m_errorString(""), m_retVal(false)
{}

void
CSVThreadLoader::run()
{
    m_errorString.clear();
    //-----------------------------------------------------
    if (m_fileNames.size() > 0 ) {
        emit sendMessage( tr("Preparing to read the file(s).") );
        m_retVal = beginTrancaction();
        if (m_retVal) {
            m_retVal = prepareRequest(insertOriginalData);
            if (m_retVal) {
                qsizetype filesCount = m_fileNames.size();
                QString fileName;
                for (qsizetype i = 0; i < filesCount; ++i) {
                    fileName = m_fileNames.at(i);
                    emit sendMessage( tr("Reading of the file %1 has started.").arg(fileName) );
                    setFileName(fileName);
                    m_retVal = read();
                    if (m_retVal) {
                        m_retVal = commit();
                        if (!m_retVal) {
                            break;
                        }
                    }
                    emit sendMessage( tr("The file %1 was read").arg(fileName) );
                } //for
            }
        }
        closeDB();
    } // m_fileNames.size()
    //-----------------------------------------------------
    if (!m_retVal) {
        m_errorString = errorString();
    }
}
