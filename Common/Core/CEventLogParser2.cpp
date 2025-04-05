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

#include "CEventLogParser2.h"
#include <QRegularExpression>

#include "DBStrings.h"

namespace {
    const QRegularExpression reEventLog2Header(QLatin1String("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")"));
    const QRegularExpression reSuccessLogon(QLatin1String("^username:\\s(.*?),\\n\\s\\stype:\\s(.*?),\\n\\s\\sip\\saddress:\\s(.*?)$"));
    const QRegularExpression reFailedLogon(QLatin1String("^username:\\s(.*?),\\n\\stype:\\s(.*?),\\n\\sip\\saddress:\\s(.*?)$"));

    const QString authSuccessUk("Вхід користувача - успішно");
    const QString authSuccessEn("User login - successful");
    const QString authFailedUk("Вхід користувача - невдало");
    const QString authFailedEn("User login - unsuccessful");
}

bool
CEventLogParser2::parseUserSuccessLogonDetails()
{
    QRegularExpressionMatch match = reSuccessLogon.match(m_details);
    if (!match.hasMatch()) {
        return false;
    }
    m_username1 = match.captured(1).trimmed();
    m_authType = match.captured(2).trimmed();
    m_ipaddresses = match.captured(3).trimmed();
    analizeIPAdresses();
    return true;
}

bool
CEventLogParser2::parseUserFailedLogonDetails()
{
    QRegularExpressionMatch match = reFailedLogon.match(m_details);

    bool retVal = match.hasMatch();
    if (retVal) {
        m_username1 = match.captured(1).trimmed();
        if (m_username1 == "null") {
            m_username1.clear();
        }
        m_authType = match.captured(2).trimmed();
        m_ipaddresses = match.captured(3).trimmed();
        analizeIPAdresses();
    } else {
        m_username1.clear();
        m_authType.clear();
        m_externalip.clear();
        m_internalip.clear();
    }
    return retVal;
}

bool
CEventLogParser2::userSuccessLogonDetails()
{
    bool retVal = true;
    if (QString::compare(m_prevValueUSLD.details, m_details, Qt::CaseInsensitive) == 0) {
        m_username1 = m_prevValueUSLD.username;
        m_authType = m_prevValueUSLD.authType;
        m_internalip = m_prevValueUSLD.internalIp;
        m_externalip = m_prevValueUSLD.externalIp;
    } else {
        retVal = parseUserSuccessLogonDetails();
        if (retVal) {
            m_prevValueUSLD.details = m_details;
            m_prevValueUSLD.username = m_username1;
            m_prevValueUSLD.authType = m_authType;
            m_prevValueUSLD.internalIp = m_internalip;
            m_prevValueUSLD.externalIp = m_externalip;
        }
    }
    return retVal;
}

bool
CEventLogParser2::userFailedLogonDetails()
{
    bool retVal = true;
    if (QString::compare(m_prevValueUFLD.details, m_details, Qt::CaseInsensitive) == 0) {
        m_username1.clear();
        m_authType = m_prevValueUFLD.authType;
        m_internalip = m_prevValueUFLD.internalIp;
        m_externalip = m_prevValueUFLD.externalIp;
    } else {
        retVal = parseUserFailedLogonDetails();
        if (retVal) {
            m_prevValueUFLD.details = m_details;
            m_prevValueUFLD.authType = m_authType;
            m_prevValueUFLD.internalIp = m_internalip;
            m_prevValueUFLD.externalIp = m_externalip;
        }
    }
    return retVal;
}

bool
CEventLogParser2::parseUserLogonDetails()
{
    bool retVal = false;
    if ((QString::compare(m_type, authSuccessUk, Qt::CaseInsensitive) == 0) ||
        (QString::compare(m_type, authSuccessEn, Qt::CaseInsensitive) == 0)) {
        retVal = userSuccessLogonDetails();
    } else if ((QString::compare(m_type, authFailedUk, Qt::CaseInsensitive) == 0) ||
               (QString::compare(m_type, authFailedEn, Qt::CaseInsensitive) == 0)) {
        retVal = userFailedLogonDetails();
    }
    return retVal;
}

CEventLogParser2::CEventLogParser2(QObject *parent)
    : CBasicParser{parent}
{
    clearErrorString();
    initFfs(m_eolChars, m_quoteChar, m_delimiterChar);
    setQuoteChar(m_quoteChar);
}

bool
CEventLogParser2::parse(const QString &line)
{
    m_details = line;
    QRegularExpressionMatch match = reEventLog2Header.match(m_details);
    if (!match.hasMatch()) {
        setErrorString(QStringLiteral("Wrong header.\nDetails: %1").arg(m_details));
        return false;
    }

    m_header = match.captured(1);
    m_username = match.captured(2);
    m_timestampISO8601 = match.captured(3);
    m_sessionID = match.captured(4);
    m_requestID = match.captured(5);
    m_type = match.captured(6);

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
CEventLogParser2::convertData(QMap<QString, QVariant> &data)
{
    data[phUsername] = m_username;
    data[phTimestampISO8601] = m_timestampISO8601;
    data[phTimestamp] = m_timestamptz;
    data[phSessionID] = m_sessionID;
    data[phRequestID] = m_requestID;
    data[phType] = m_type;
    data[phDetails] = m_details;
    data[phUsername1] = m_username1;
    data[phAuthType] = m_authType;
    data[phExternalip] = m_externalip;
    data[phInternalip] = m_internalip;
}

QString
CEventLogParser2::insertString() const
{
    return eventlog2::insertData;
}

void
CEventLogParser2::getParsedData(QString &username,
                                QString &timestampISO8601,
                                QString &sessionID,
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
    sessionID = m_sessionID;
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
CEventLogParser2::createTable() const
{
    return QString();
}
