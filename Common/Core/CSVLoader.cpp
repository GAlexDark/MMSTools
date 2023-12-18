#include "CSVLoader.h"

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

bool
CSVLoader::initDB(const QString &dbFileName)
{
    bool retVal = m_db.init("QSQLITE", dbFileName);
    if (retVal) {
        retVal = m_db.open();
        if (retVal) {
            retVal = m_db.exec("PRAGMA journal_mode = MEMORY;");
            if (retVal) {
                retVal = m_db.exec("PRAGMA page_size = 32768;");
                if (retVal) {
                    retVal = m_db.exec("PRAGMA synchronous = OFF;");
                    if (retVal) {
                        retVal = m_db.exec("PRAGMA cache_size = 100000;"); // from test
                        if (retVal) {
                            retVal = m_db.exec("PRAGMA temp_store = MEMORY;"); // from test
                            if (retVal) {
                                retVal = m_db.exec(createEventLogTable);
                            }
                        }
                    }
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
            m_buffer = new QByteArray;
            Q_ASSERT(m_buffer);
            m_buffer->resize(m_bufferSize);
            memset(m_buffer->data(), 0, m_buffer->size());
        } else {
            retVal = false;
            m_errorString = "Invalid buffer size";
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

CSVLoader::CSVLoader()
{
    __DEBUG( Q_FUNC_INFO )

    m_errorString.clear();
    m_fileName.clear();
    m_eolChars.clear();
    m_buffer = nullptr;
    m_bufferSize = defBufferSize;
    m_fileNames.clear();
    m_isHeaders = false;
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
CSVLoader::init(const QString &dbFileName, bool dataHasHeaders, const QByteArray &eolChars, qint64 bufferSize)
{
    __DEBUG( Q_FUNC_INFO )

    m_eolChars = eolChars;
    m_isHeaders = dataHasHeaders;
    m_bufferSize = bufferSize;

    bool retVal = initDB(dbFileName);
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
                    //m_callbackFunction(line);

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


void
CSVThreadLoader::run()
{
    __DEBUG( Q_FUNC_INFO )

    bool retVal = true;
    //-----------------------------------------------------
    if (m_fileNames.size() > 0 ) {
        retVal = beginTrancaction();
        if (retVal) {
            retVal = prepareRequest(insertOriginalData);
            if (retVal) {
                qsizetype filesCount = m_fileNames.size();
                for (qsizetype i = 0; i < filesCount; ++i) {
                    setFileName(m_fileNames.at(i));
                    retVal = read();
                    if (retVal) {
                        retVal = commit();
                        if (!retVal) {
                            break;
                        }
                    }
                } //for
            }
        }
        closeDB();
    } // m_fileNames.size()
    //-----------------------------------------------------
    if (!retVal) {
        m_errorString = errorString();
        __DEBUG( m_errorString )
    }
}
