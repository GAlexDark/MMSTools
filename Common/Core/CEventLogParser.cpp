/****************************************************************************
*
* Class for parsing the MMS Event logs.
* Copyright (C) 2024  Oleksii Gaienko
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

#include "CEventLogParser.h"
#include <QRegularExpression>

#include "DBStrings.h"

namespace {
    const QRegularExpression reEventLogHeader(QLatin1String("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")"));
    const QRegularExpression reFailedLogon(QLatin1String("^type:\\s(.*?)\\n\\s\\sip\\saddress:\\s(.*?)$"));
}

bool
CEventLogParser::parseUserFailedLogonDetails()
{
    QRegularExpressionMatch match = reFailedLogon.match(m_details);
    m_username1.clear();
    bool retVal = match.hasMatch();
    if (retVal) {
        m_authType = match.captured(1).trimmed();
        m_ipaddresses = match.captured(2).trimmed();
        analizeIPAdresses();
    } else {
        m_authType.clear();
        m_externalip.clear();
        m_internalip.clear();
    }
    return retVal;
}

CEventLogParser::CEventLogParser(QObject *parent)
    : CEventLogParserBase{parent}
{
    initFfs(m_eolChars, m_quoteChar, m_delimiterChar);
    setQuoteChar(m_quoteChar);
}

bool
CEventLogParser::parse(const QString &line)
{
    m_details = line;
    QRegularExpressionMatch match = reEventLogHeader.match(m_details);
    if (!match.hasMatch()) {
        setErrorString(QStringLiteral("Wrong header.\nDetails: %1").arg(m_details));
        return false;
    }

    m_header = match.captured(1);
    m_username = match.captured(2);
    m_timestampISO8601 = match.captured(3);
    m_requestID = match.captured(4);
    m_type = match.captured(5);

    m_details = m_details.mid(m_header.length() + 1);
    removeQuote(m_details);

    bool retVal = true;
    m_timestamp = QDateTime::fromString(m_timestampISO8601, Qt::ISODateWithMs);
    if (m_timestamp.isValid()) {
        m_timestamp.setTimeSpec(Qt::UTC);
        m_timestamptz = m_timestamp.toLocalTime();
    } else {
        m_timestamptz = QDateTime();
        setErrorString(QStringLiteral("Error converting Timestamp value: %1").arg(m_timestampISO8601));
        retVal = false;
    }
    if (!parseUserLogonDetails()) {
        m_username1.clear();
        m_authType.clear();
        m_externalip.clear();
        m_internalip.clear();
    }
    return retVal;
}

void
CEventLogParser::convertData(QMap<QString, QVariant> &data)
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
#ifdef QT_DEBUG
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
                   QDateTime &timestampTZ) const
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
#endif
QString
CEventLogParser::createTable() const
{
    return eventlog::createTable;
}
