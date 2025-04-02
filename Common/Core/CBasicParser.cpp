/****************************************************************************
*
* The Base class for the MMS log parsers
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

#include "CBasicParser.h"
#include <QMetaObject>
#include "elcUtils.h"

void
CBasicParser::removeQuote(QString &data) const
{
    if (data.startsWith(m_quoteChar) && data.endsWith(m_quoteChar)) {
        data = data.sliced(1, data.length() - 2);
    }
}

CBasicParser::CBasicParser(QObject *parent)
    : QObject{parent}
{
    m_eolChars.clear();
}

void
CBasicParser::init(const QString &internalIpFirstOctet)
{
    m_internalIpFirstOctet = internalIpFirstOctet;
}

void
CBasicParser::analizeIPAdresses()
{
    const qsizetype pos = m_ipaddresses.indexOf(',');
    m_externalip.clear();
    m_internalip.clear();
    QString buf;
    if (pos != -1) {
        m_externalIpList.clear();
        m_internalIpList.clear();
        m_ipAddressList.clear();
        m_ipAddressList = m_ipaddresses.split(',');
        for (const QString &item : m_ipAddressList) {
            buf = item.trimmed();
            if (buf.startsWith(m_internalIpFirstOctet)) {
                m_internalIpList.append(buf);
            } else {
                m_externalIpList.append(buf);
            }
        }
        m_externalip = m_externalIpList.join(", ");
        m_internalip = m_internalIpList.join(", ");
    } else {
        buf = m_ipaddresses.trimmed();
        if (buf.startsWith(m_internalIpFirstOctet)) {
            m_internalip = buf;
        } else {
            m_externalip = buf;
        }
    }
}

void
CBasicParser::initFfs(QByteArray &eolChars, char &quoteChar, char &delimiterChar)
{
    QString buf;
    bool retVal = elcUtils::getMetaClassInfo(this, "eolChars", buf);
    Q_ASSERT(retVal);
    eolChars = buf.toUtf8();
    retVal = elcUtils::getMetaClassInfo(this, "quoteChar", buf);
    Q_ASSERT(retVal);
    quoteChar = buf.at(0).toLatin1();
    retVal = elcUtils::getMetaClassInfo(this, "delimiterChar", buf);
    Q_ASSERT(retVal);
    delimiterChar = buf.at(0).toLatin1();
}

QString
CBasicParser::tableName() const
{
    QString retVal;
    return elcUtils::getMetaClassInfo(this, QLatin1String("tablename"), retVal) ? retVal : QString();
}

quint16
CBasicParser::parserID() const
{
    QString retVal;
    return elcUtils::getMetaClassInfo(this, QLatin1String("ID"), retVal) ? retVal.toUInt() : 0;
}
