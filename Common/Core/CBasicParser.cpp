/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CBasicParser.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the parsers
*
****************************************************************************/

#include "CBasicParser.h"
#include <QMetaObject>
#include "elcUtils.h"

void
CBasicParser::removeQuote(QString &data) const
{
    if (!data.isEmpty() && data.startsWith(m_quoteChar) && data.endsWith(m_quoteChar)) {
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
    qsizetype pos = m_ipaddresses.indexOf(',');
    m_externalip.clear();
    m_internalip.clear();
    if (pos != -1) {
        QString buf;
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
        if (!m_externalIpList.isEmpty()) {
            m_externalip = m_externalIpList.join(", ");
        }
        if (!m_internalIpList.isEmpty()) {
            m_internalip = m_internalIpList.join(", ");
        }
    } else {
        if (m_ipaddresses.startsWith(m_internalIpFirstOctet)) {
            m_internalip = m_ipaddresses;
        } else {
            m_externalip = m_ipaddresses;
        }
    } // (pos != -1)
}

void
CBasicParser::initFfs(QByteArray &eolChars, char &quoteChar, char &delimiterChar)
{
    QString buf;
    elcUtils::getMetaClassInfo(this, "eolChars", buf);
    eolChars = buf.toUtf8();
    elcUtils::getMetaClassInfo(this, "quoteChar", buf);
    quoteChar = buf.at(0).toLatin1();
    elcUtils::getMetaClassInfo(this, "delimiterChar", buf);
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
