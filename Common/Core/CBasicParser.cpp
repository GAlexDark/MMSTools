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
#include "elcUtils.h"
#include <QMetaObject>

void CBasicParser::removeQuote(QString &data) const {
  if (!data.isEmpty() && data.startsWith(m_quoteChar) &&
      data.endsWith(m_quoteChar)) {
    data = data.sliced(1, data.length() - 2);
  }
}

CBasicParser::CBasicParser(QObject *parent) : QObject{parent} {
  m_eolChars.clear();
}

void CBasicParser::init(const QString &internalIpFirstOctet) {
  m_internalIpFirstOctet = internalIpFirstOctet;
}

void CBasicParser::analizeIPAdresses() {
  qsizetype pos = m_ipaddresses.indexOf(',');
  if (pos != -1) {
    QString firstip = m_ipaddresses.sliced(0, pos).trimmed();
    QString secondip = m_ipaddresses.mid(pos + 1).trimmed();
    bool isPrivateFirstIP = firstip.startsWith(m_internalIpFirstOctet);
    bool isPrivateSecondIP = secondip.startsWith(m_internalIpFirstOctet);

    if (isPrivateFirstIP && isPrivateSecondIP) {
      m_externalip.clear();
      m_internalip = m_ipaddresses;
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
    if (m_ipaddresses.startsWith(m_internalIpFirstOctet)) {
      m_externalip.clear();
      m_internalip = m_ipaddresses;
    } else {
      m_externalip = m_ipaddresses;
      m_internalip.clear();
    }
  } // (pos != -1)
}

void CBasicParser::initFfs(QByteArray &eolChars, char &quoteChar,
                           char &delimiterChar) {
  QString buf;
  elcUtils::getMetaClassInfo(this, "eolChars", buf);
  eolChars = buf.toUtf8();
  elcUtils::getMetaClassInfo(this, "quoteChar", buf);
  quoteChar = buf.at(0).toLatin1();
  elcUtils::getMetaClassInfo(this, "delimiterChar", buf);
  delimiterChar = buf.at(0).toLatin1();
}

QString CBasicParser::tableName() const {
  QString retVal;
  return elcUtils::getMetaClassInfo(this, QLatin1String("tablename"), retVal)
             ? retVal
             : QString();
}

quint16 CBasicParser::parserID() const {
  QString retVal;
  return elcUtils::getMetaClassInfo(this, QLatin1String("ID"), retVal)
             ? retVal.toUInt()
             : 0;
}
