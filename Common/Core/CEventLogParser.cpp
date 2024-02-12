/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CEventLogParser.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for MMS Event logs parsing.
*
****************************************************************************/


#include "CEventLogParser.h"
#include <QRegularExpression>

//#include "Debug.h"

QRegularExpression reHeader("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")");
QRegularExpression reSuccessLogon("^username:\\s(.*?),@N@\\s\\stype:\\s(.*?),@N@\\s\\sip\\saddress:\\s(.*?)$");
QRegularExpression reFailedLogon("^type:\\s(.*?)@N@\\s\\sip\\saddress:\\s(.*?)$");

inline QString authSuccess = QStringLiteral("Вхід користувача - успішно");
inline QString authFailed = QStringLiteral("Вхід користувача - невдало");

void
CEventLogParser::removeQuote(QString &data, QChar quoteChar)
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
CEventLogParser::parseHeaderString(QChar quoteChar)
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
CEventLogParser::analizeIPAdresses(const QString &ipaddresses)
{
    qsizetype pos = ipaddresses.indexOf(',');
    if (pos != -1) {
        QString firstip = ipaddresses.mid(0, pos).trimmed();
        QString secondip = ipaddresses.mid(pos + 1).trimmed();
        bool isPrivateFirstIP = firstip.startsWith(m_internalIpFirstOctet);
        bool isPrivateSecondIP = secondip.startsWith(m_internalIpFirstOctet);

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
        if (ipaddresses.startsWith(m_internalIpFirstOctet)) {
            m_externalip.clear();
            m_internalip = ipaddresses;
        } else {
            m_externalip = ipaddresses;
            m_internalip.clear();
        }
    } // (pos != -1)
}

bool
CEventLogParser::parseUserSuccessLogonDetails()
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
CEventLogParser::parseUserFailedLogonDetails()
{
    QRegularExpressionMatch match = reFailedLogon.match(m_details);
    m_username1.clear();
    bool retVal = match.hasMatch();
    if (retVal) {
        m_authType = match.captured(1).trimmed();

        QString ipaddresses = match.captured(2).trimmed();
        analizeIPAdresses(ipaddresses);
    } else {
        m_authType.clear();
        m_externalip.clear();
        m_internalip.clear();
    }
    return retVal;
}

bool
CEventLogParser::parseUserLogonDetails()
{
    bool retVal = false;
    if (QString::compare(m_type, authSuccess, Qt::CaseInsensitive) == 0) {
        retVal = parseUserSuccessLogonDetails();
    } else {
        if (QString::compare(m_type, authFailed, Qt::CaseInsensitive) == 0) {
            retVal = parseUserFailedLogonDetails();
        }
    }
    return retVal;
}

void
CEventLogParser::init(const QString &internalIpFirstOctet)
{
    m_internalIpFirstOctet = internalIpFirstOctet;
}

bool
CEventLogParser::parse(const QString &line)
{
    m_details = line;
    m_details.replace('\n', QLatin1String("@N@"), Qt::CaseInsensitive);

    bool retVal = false;
    QRegularExpressionMatch match = reHeader.match(m_details);
    if (match.hasMatch()) {
        m_header = match.captured(0);
        parseHeaderString('"');

        m_details = m_details.mid(m_header.length() + 1);
        removeQuote(m_details, '"');

        m_timestamp = QDateTime::fromString(m_timestampISO8601, Qt::ISODateWithMs);
        if (m_timestamp.isValid()) {
            m_timestamp.setTimeSpec(Qt::UTC);
            m_timestamptz = m_timestamp.toLocalTime();
            retVal = true;
        } else {
            m_timestamptz = QDateTime();
            m_errorString = QStringLiteral("Error converting Timestamp value: %1").arg(m_timestampISO8601);
        }
        if (!parseUserLogonDetails()) {
            m_username1.clear();
            m_authType.clear();
            m_externalip.clear();
            m_internalip.clear();
        }
    } else {
        m_errorString = QStringLiteral("Wrong header.\nDetails: %1").arg(m_details);
    }
    return retVal;
}

void
CEventLogParser::getParsedData(QString &username,
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


