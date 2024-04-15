#include "CBasicParser.h"
#include <QMetaObject>
#include <QMetaClassInfo>

void
CBasicParser::removeQuote(QString &data, char quoteChar)
{
    if (!data.isEmpty() && data.startsWith(quoteChar) && data.endsWith(quoteChar)) {
        data = data.sliced(1, data.length() - 2);
    }
}

void
CBasicParser::init(const QString &internalIpFirstOctet)
{
    m_internalIpFirstOctet = internalIpFirstOctet;
}

void
CBasicParser::analizeIPAdresses(const QString &ipaddresses)
{
    qsizetype pos = ipaddresses.indexOf(',');
    if (pos != -1) {
        QString firstip = ipaddresses.sliced(0, pos).trimmed();
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

QString
CBasicParser::tableName() const
{
    QString retVal;
    const QMetaObject* mObj = this->metaObject();
    for (int j = mObj->classInfoOffset(); j < mObj->classInfoCount(); j++) {
        QMetaClassInfo classInfo = mObj->classInfo(j);
        if (QString(classInfo.name()) == QLatin1String("tablename")) {
            retVal = classInfo.value();
            break;
        }
    }
    return retVal;
}

quint16
CBasicParser::parserID() const
{
    quint16 retVal = 0;
    const QMetaObject* mObj = this->metaObject();
    for (int j = mObj->classInfoOffset(); j < mObj->classInfoCount(); j++) {
        QMetaClassInfo classInfo = mObj->classInfo(j);
        if (QString(classInfo.name()) == QLatin1String("ID")) {
            retVal = QString(classInfo.value()).toUInt();
            break;
        }
    }
    return retVal;
}

mms::ffs_t
CBasicParser::fileFieldsSeparationInfo()
{
    mms::ffs_t retVal{ m_delimiterChar, m_quoteChar, m_eolChars };
    return retVal;
}
