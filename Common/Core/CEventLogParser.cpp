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

#include "DBStrings.h"

//#include "Debug.h"

QRegularExpression reEventLogHeader("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")");
QRegularExpression reSuccessLogon("^username:\\s(.*?),\\n\\s\\stype:\\s(.*?),\\n\\s\\sip\\saddress:\\s(.*?)$");
QRegularExpression reFailedLogon("^type:\\s(.*?)\\n\\s\\sip\\saddress:\\s(.*?)$");

const QString authSuccessUk("Вхід користувача - успішно");
const QString authSuccessEn("User login - successful");
const QString authFailedUk("Вхід користувача - невдало");
const QString authFailedEn("User login - unsuccessful");

//=================================================================================

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
    if ((QString::compare(m_type, authSuccessUk, Qt::CaseInsensitive) == 0) ||
        (QString::compare(m_type, authSuccessEn, Qt::CaseInsensitive) == 0)) {
        retVal = parseUserSuccessLogonDetails();
    } else {
        if ((QString::compare(m_type, authFailedUk, Qt::CaseInsensitive) == 0) ||
            (QString::compare(m_type, authFailedEn, Qt::CaseInsensitive) == 0)) {
            retVal = parseUserFailedLogonDetails();
        }
    }
    return retVal;
}

CEventLogParser::CEventLogParser()
{
    m_delimiterChar = ',';
    m_quoteChar = '"';
    m_eolChars = "\r\n";
}

bool
CEventLogParser::parse(const QString &line)
{
    m_details = line;

    bool retVal = false;
    QRegularExpressionMatch match = reEventLogHeader.match(m_details);
    if (match.hasMatch()) {
        m_header = match.captured(1);
        m_username = match.captured(2);
        m_timestampISO8601 = match.captured(3);
        m_requestID = match.captured(4);
        m_type = match.captured(5);

        m_details = m_details.mid(m_header.length() + 1);
        removeQuote(m_details, m_quoteChar);

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
CEventLogParser::convertData(mms::dataItem_t &data)
{
    data[phUsername] = m_username;
    data[phTimestampISO8601] = m_timestampISO8601;
    data[phTimestamp] = m_timestamptz;
    data[phRequestID] = m_requestID;
    data[phType] = m_type;
    data[phDetails] = m_details;
    data[phUsername1] = m_username1;
    data[phAuthType] = m_authType;
    data[phExternalip] = m_externalip;
    data[phInternalip] = m_internalip;
}

QString
CEventLogParser::insertString() const
{
    return eventlog::insertData;
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

QString
CEventLogParser::createTable() const
{
    return eventlog::createTable;
}