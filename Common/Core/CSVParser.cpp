#include "CSVParser.h"
#include <QRegularExpression>

#include "Debug.h"

QRegularExpression reHeader("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")");
QRegularExpression reSuccessLogon("^username:\\s(.*?),@N@\\s\\stype:\\s(.*?),@N@\\s\\sip\\saddress:\\s(.*?)$");
QRegularExpression reFailedLogon("^type:\\s(.*?)@N@\\s\\sip\\saddress:\\s(.*?)$");

const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
                                    timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
                                    type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, \
                                    internalip TEXT, timestamp DATETIME, \
                                    PRIMARY KEY (timestampISO8601, requestid));");
/*
const qsizetype userNameLen = QString("username: ").length();
const qsizetype typeLen = QString("type: ").length();
const qsizetype ipaddressLen = QString("ip address: ").length();
*/
void
CSVParser::removeQuote(QString &data, QChar quoteChar)
{    
    if (!data.isEmpty()) {
        if (data.startsWith(quoteChar)) {
            data = data.mid(1);
        }
        qsizetype lastChar = data.length() - 1;
        if (data.endsWith(quoteChar)) {
            data = data.left(lastChar);
        }
    }
}

void
CSVParser::parseHeaderString(QChar quoteChar)
{
    qsizetype from = 0;
    qsizetype next = m_header.indexOf(',', from);
    m_username = m_header.mid(from, next);
    removeQuote(m_username, quoteChar);
    from = next +1;

    next = m_header.indexOf(',', from);
    m_timestampISO8601 = m_header.mid(from, next - from);
    removeQuote(m_timestampISO8601, quoteChar);
    from = next +1;

    next = m_header.indexOf(',', from);
    m_requestID = m_header.mid(from, next - from);
    removeQuote(m_requestID, quoteChar);
    from = next +1;

    next = m_header.indexOf(',', from);
    m_type = m_header.mid(from, next - from);
    removeQuote(m_type, quoteChar);
}

//=================================================================================
void
CSVParser::analizeIPAdresses(const QString &ipaddresses)
{
    qsizetype pos = ipaddresses.indexOf(',');
    if (pos != -1) {
        QString firstip = ipaddresses.mid(0, pos).trimmed();
        QString secondip = ipaddresses.mid(pos + 1).trimmed();
        bool isPrivateFirstIP = (firstip.indexOf(m_internalipFirstOctet) == 0)? true : false;
        bool isPrivateSecondIP = (secondip.indexOf(m_internalipFirstOctet) == 0)? true : false;

        if (isPrivateFirstIP && isPrivateSecondIP) {
            m_externalip.clear();
            m_internalip = ipaddresses;
        } else {
            if (isPrivateFirstIP) {
                m_externalip = secondip;
                m_internalip = firstip;
            } else {
                m_externalip = firstip;
                m_internalip = secondip;
            }
        } // if &&
    } else {
        if (ipaddresses.indexOf(m_internalipFirstOctet) == 0) {
            m_externalip.clear();
            m_internalip = ipaddresses;
        } else {
            m_externalip = ipaddresses;
            m_internalip.clear();
        }
    } // (pos != -1)
}

bool
CSVParser::parseUserSuccessLogonDetails()
{
    QRegularExpressionMatch match = reSuccessLogon.match(m_details);
    bool retVal = match.hasMatch();
    if (retVal) {
        m_username1 = match.captured(1).trimmed();

        m_authType = match.captured(2).trimmed();

        QString ipaddresses = match.captured(3).trimmed();
        analizeIPAdresses(ipaddresses);
    }
    return retVal;
}

bool
CSVParser::parseUserFailedLogonDetails()
{
    QRegularExpressionMatch match = reFailedLogon.match(m_details);
    bool retVal = match.hasMatch();
    if (retVal) {
        m_username1.clear();

        m_authType = match.captured(1).trimmed();

        QString ipaddresses = match.captured(2).trimmed();
        analizeIPAdresses(ipaddresses);
    }
    return retVal;
}

bool
CSVParser::parseUserLogonDetails()
{
    bool retVal = parseUserSuccessLogonDetails();
    if (!retVal) {
        retVal = parseUserFailedLogonDetails();
    }
    return retVal;
}

//=================================================================================
CSVParser::CSVParser()
{
    __DEBUG( Q_FUNC_INFO )
}

void
CSVParser::init(const QString &internalipFirstOctet)
{
    m_internalipFirstOctet = internalipFirstOctet;
}

void
CSVParser::parse(const QString &line)
{
    m_details = line;
    m_details.replace("\n", "@N@", Qt::CaseInsensitive);

    QRegularExpressionMatch match = reHeader.match(m_details);
    if (match.hasMatch()) {
        m_header = match.captured(0);
        parseHeaderString('"');

        m_details = m_details.mid(m_header.length() + 1);
        removeQuote(m_details, '"');

        m_timestamp = QDateTime::fromString(m_timestampISO8601, Qt::ISODateWithMs);
        m_timestamp.setTimeSpec(Qt::UTC);
        m_timestamptz = m_timestamp.toLocalTime();

        if ((m_details.indexOf("ip address:") == -1) || !parseUserLogonDetails()) {
            m_username1.clear();
            m_authType.clear();
            m_externalip.clear();
            m_internalip.clear();
        }
    }
}

void
CSVParser::getParsedData(QString &username,
                   QString &timestampISO8601,
                   QString &requestID,
                   QString &type,
                   QString &details,
                   QString &username1,
                   QString &authType,
                   QString &externalIP,
                   QString &internalIP,
                   QDateTime &timestampTZ)
{
    username = m_username;
    timestampISO8601 = m_timestampISO8601;
    requestID = m_requestID;
    type = m_type;
    details = m_details;
    username1 = m_username1;
    authType = m_authType;
    externalIP = m_externalip;
    internalIP = m_internalip;
    timestampTZ = m_timestamptz;
}


