#include "CSVParser.h"
#include <QRegularExpression>

#include "Debug.h"


const QString format24 = "yyyy-MM-ddTHH:mm:ss.zzzZ"; // lenght 24
const QString format22 = "yyyy-MM-ddTHH:mm:ss.zZ"; // lenght 22
const QString format20 = "yyyy-MM-ddTHH:mm:ssZ"; //lenght 20

QRegularExpression reHeader("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")"); //get header
QRegularExpression reSuccessLogon("^(.*?)@N@(.*?)@N@(.*?)$");
QRegularExpression reFailedLogon("^(.*?)@N@(.*?)$");

const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
                                    timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
                                    type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, \
                                    internalip TEXT, timestamp DATETIME, \
                                    PRIMARY KEY (timestampISO8601, requestid));");

void
CSVParser::removeQuote(QString &data, QChar quoteChar)
{    
    if (!data.isEmpty()) {
        if (data.at(0) == quoteChar) {
            data.remove(0, 1);
        }
        int lastChar = data.length() - 1;
        if (data.at(lastChar) == quoteChar) {
            data.resize(lastChar);
        }
    }
}

void
CSVParser::parseHeaderString(QChar quoteChar)
{
    int from = 0;
    int next = 0;

    next = m_header.indexOf(',', from);
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
CSVParser::analizeIPAdresses(const QString &ipaddresses, QString &externalIP, QString &internalIP)
{
    int pos = ipaddresses.indexOf(',');
    if (pos != -1) {
        QString firstip = ipaddresses.mid(0, pos).trimmed();
        QString secondip = ipaddresses.mid(pos + 1).trimmed();
        bool isPrivateFirstIP = (firstip.indexOf("10.") == 0)? true : false;
        bool isPrivateSecondIP = (secondip.indexOf("10.") == 0)? true : false;

        if (isPrivateFirstIP && isPrivateSecondIP) {
            externalIP.clear();
            internalIP = ipaddresses;
            return;
        }

        if (isPrivateFirstIP) {
            externalIP = secondip;
            internalIP = firstip;
        } else {
            externalIP = firstip;
            internalIP = secondip;
        }
        return;
    } // pos()

    if (ipaddresses.indexOf("10.") == 0) {
        externalIP.clear();
        internalIP = ipaddresses;
    } else {
        externalIP = ipaddresses;
        internalIP.clear();
    }
}

bool
CSVParser::parseUserSuccessLogonDetails()
{
    bool retVal = false;

    QRegularExpressionMatch match = reSuccessLogon.match(m_details);
    if (match.hasMatch()) {
        m_username1 = match.captured(1).trimmed();
        m_username1.remove("username: ");
        m_username1.resize(m_username1.length() - 1);

        m_authType = match.captured(2).trimmed();
        m_authType.remove("type: ");
        m_authType.resize(m_authType.length() - 1);

        QString ipaddresses = match.captured(3).trimmed();
        ipaddresses.remove("ip address: ");
        analizeIPAdresses(ipaddresses, m_externalip, m_internalip);

        retVal = true;
    }
    return retVal;
}

bool
CSVParser::parseUserFailedLogonDetails()
{
    bool retVal = false;
    m_username1.clear();

    QRegularExpressionMatch match = reFailedLogon.match(m_details);
    if (match.hasMatch()) {
        m_authType = match.captured(1).trimmed();
        m_authType.remove("type: ");

        QString ipaddresses = match.captured(2).trimmed();
        ipaddresses.remove("ip address: ");
        analizeIPAdresses(ipaddresses, m_externalip, m_internalip);

        retVal = true;
    }
    return retVal;
}

bool
CSVParser::parseUserLogonDetails()
{
    bool retVal = parseUserSuccessLogonDetails();;
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
CSVParser::parse(const QString &line)
{
    __DEBUG( Q_FUNC_INFO )

    m_details = line;
    m_details.replace("\n", "@N@", Qt::CaseInsensitive);

    QRegularExpressionMatch match = reHeader.match(m_details);
    if (match.hasMatch()) {
        m_header = match.captured(0);
        parseHeaderString('"');

        m_details.remove(m_header, Qt::CaseInsensitive);
        m_details.remove(0, 1); // remove first ',' char before quote char in details field
        removeQuote(m_details, '"');

        switch (m_timestampISO8601.length()) {
        case 20:
            m_format = format20;
            break;
        case 22:
        case 23:
            m_format = format22;
            break;
        default:
            m_format = format24;
            break;
        }
        m_timestamp = QDateTime::fromString(m_timestampISO8601, m_format);
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


