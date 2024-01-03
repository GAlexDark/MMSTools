#include "CSVLoader.h"

#include "QStorageInfoHelper.h"
#include "Debug.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVariant nullStringValue = QVariant(QMetaType::fromType<QString>()); // Qt 6 only
#endif

const QString insertOriginalData = QStringLiteral("INSERT OR IGNORE INTO [eventlog] (username, timestampISO8601, \
                                    requestid, type, details, timestamp, username1, authtype, externalip, \
                                    internalip) VALUES (:username, :timestampISO8601, :requestid, :type, \
                                    :details, :timestamp, :username1, :authtype, :externalip, :internalip)");

const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
                                    timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
                                    type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME, \
                                    PRIMARY KEY (timestampISO8601, requestid));");

const QString pragmaUTF8 = QStringLiteral("PRAGMA encoding = \"UTF-8\";");
const QString pragmaJournalMode = QStringLiteral("PRAGMA journal_mode = %1;");
const QString pragmaPageSize = QStringLiteral("PRAGMA page_size = %1;");
const QString pragmaSynchronous = QStringLiteral("PRAGMA synchronous = NORMAL;");
const QString pragmaTempStore = QStringLiteral("PRAGMA temp_store = %1;");


bool
CSVLoader::initDB(const QString &dbFileName, const QString &tempStore, const QString &journalMode)
{    
    bool retVal = m_db.init("QSQLITE", dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            int blockSize = QStorageInfoHelper::getStorageBlockSize(dbFileName);
            QStringList pragmaItems;
            pragmaItems.append(pragmaUTF8);
            if (QString::compare(journalMode, "memory", Qt::CaseInsensitive) == 0) {
                pragmaItems.append(pragmaJournalMode.arg("MEMORY"));
            } else {
                pragmaItems.append(pragmaJournalMode.arg("DELETE"));
            }
            pragmaItems.append(pragmaPageSize.arg(blockSize));
            pragmaItems.append(pragmaSynchronous);
            if (QString::compare(tempStore, "memory", Qt::CaseInsensitive) == 0) {
                pragmaItems.append(pragmaTempStore.arg("MEMORY"));
            } else {
                pragmaItems.append(pragmaTempStore.arg("DEFAULT"));
            }
            for (qsizetype i = 0; i < pragmaItems.size(); ++i) {
                retVal = m_db.exec(pragmaItems.at(i));
                if (!retVal) {
                    break;
                }
            }
            if (retVal) {
                retVal = m_db.exec(createEventLogTable);
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
            Q_ASSERT(m_buffer);
        } else {
            retVal = false;
            m_errorString = QStringLiteral("Invalid buffer size");
        }
    } catch (std::bad_alloc &e) {
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
    __DEBUG( Q_FUNC_INFO )

    QString buffer;
    m_file.setFileName(m_fileName);
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
    __DEBUG( Q_FUNC_INFO )

    m_file.setFileName(m_fileName);
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (retVal) {
        QString line;
        qint64 bufferPosition = 0;
        qint64 prevPosition = 0;
        qint64 nextPosition;
        qint64 eolCharsLen = m_eolChars.length();
        bool isEOF = false;
        qint64 bytesRead;

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


//-----------------------------------------------------------

CSVLoader::CSVLoader(): m_errorString(""), m_buffer(nullptr), m_bufferSize(defBufferSize), m_fileName(""), m_isHeaders(false)
{
    __DEBUG( Q_FUNC_INFO )

    m_eolChars.clear();
    m_fileNames.clear();
}

CSVLoader::~CSVLoader()
{
    __DEBUG( Q_FUNC_INFO )

    m_buffer->clear();
    delete m_buffer;

    if (m_file.isOpen()) {
        m_file.close();
    }
    closeDB();
}

bool
CSVLoader::init(const QString &dbFileName, bool dataHasHeaders, const QString &internalipFirstOctet, const QString &tempStore, const QString &journalMode,
                     const QByteArray &eolChars, qint64 bufferSize)
{
    __DEBUG( Q_FUNC_INFO )

    m_eolChars = eolChars;
    m_isHeaders = dataHasHeaders;
    m_bufferSize = bufferSize;

    m_parser.init(internalipFirstOctet);

    bool retVal = initDB(dbFileName, tempStore, journalMode);
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
    __DEBUG( Q_FUNC_INFO )

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

CSVThreadLoader::CSVThreadLoader(): m_errorString(""), m_retVal(false)
{}

void
CSVThreadLoader::run()
{
    __DEBUG( Q_FUNC_INFO )

    m_errorString.clear();
    //-----------------------------------------------------
    if (m_fileNames.size() > 0 ) {
        emit sendMessage( tr("Preparing to read file(s).") );
        m_retVal = beginTrancaction();
        if (m_retVal) {
            m_retVal = prepareRequest(insertOriginalData);
            if (m_retVal) {
                qsizetype filesCount = m_fileNames.size();
                QString fileName;
                for (qsizetype i = 0; i < filesCount; ++i) {
                    fileName = m_fileNames.at(i);
                    emit sendMessage( tr("Reading of file %1 has started.").arg(fileName) );
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
        __DEBUG( m_errorString )
    }
}
