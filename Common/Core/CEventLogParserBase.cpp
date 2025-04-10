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

#include "CEventLogParserBase.h"
#include <QRegularExpression>
#include <QString>

namespace {
	const QRegularExpression reSuccessLogon(QLatin1String("^username:\\s(.*?),\\n\\s\\stype:\\s(.*?),\\n\\s\\sip\\saddress:\\s(.*?)$"));

    const QString authSuccessUk("Вхід користувача - успішно");
    const QString authSuccessEn("User login - successful");
    const QString authFailedUk("Вхід користувача - невдало");
    const QString authFailedEn("User login - unsuccessful");
}

CEventLogParserBase::CEventLogParserBase(QObject *parent)
        : CBasicParser{parent}
{
    clearErrorString();
}

bool
CEventLogParserBase::parseUserSuccessLogonDetails()
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
CEventLogParserBase::userSuccessLogonDetails()
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
CEventLogParserBase::userFailedLogonDetails()
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
CEventLogParserBase::parseUserLogonDetails()
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
