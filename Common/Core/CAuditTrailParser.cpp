/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CAuditTrailParser.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for MMS Audit Trail logs parsing.
*
****************************************************************************/

#include "CAuditTrailParser.h"
#include <QRegularExpression>

//#include "Debug.h"
#include "DBStrings.h"

QRegularExpression reAuditTrailHeader("(^(.*?);(.*?);(.*?);(.*?);(.*?);(.*?))");
QRegularExpression rePersonData("Person\\s\\[pk=\\d+,\\salias=(.*)\\]");
QRegularExpression reLoadAuditTrail("^.*;.*;(.*);.*;.*;.*;.*;.*;.*$");

bool
CAuditTrailParser::parsePersonDataDetails()
{
    bool retVal = false;
    //__DEBUG( m_attributes )
    qsizetype firstPos = m_attributes.indexOf(person, 0);
    if (firstPos != -1) {
        qsizetype lastPos = m_attributes.indexOf(']', firstPos);
        QString alias = m_attributes.sliced(firstPos, lastPos - firstPos + 1).trimmed();
        QRegularExpressionMatch match = rePersonData.match(alias);
        if (match.hasMatch()) {
            m_username1 = match.captured(1);
            retVal = true;
        }
    }
    return retVal;
}

bool
CAuditTrailParser::parseLoadAuditTrail()
{
    // m_method == "loadAuditTrail"
    bool retVal = false;
    //__DEBUG( m_attributes )
    QRegularExpressionMatch match = reLoadAuditTrail.match(m_attributes);
    if (match.hasMatch()) {
        m_username1 = match.captured(1);
        if (QString::compare(m_username1, nullValue, Qt::CaseInsensitive) == 0) {
            m_username1.clear();
        }
        retVal = true;
    }

    return retVal;
}

bool
CAuditTrailParser::parseAttributesDetails()
{
    return parsePersonDataDetails();
}

CAuditTrailParser::CAuditTrailParser()
{
    m_eolChars = "\n";
    m_delimiterChar = ';';
    m_quoteChar = '"';
}

bool
CAuditTrailParser::parse(const QString& line)
{
    bool retVal = false;
    QRegularExpressionMatch match = reAuditTrailHeader.match(line);
    if (match.hasMatch()) {
        m_header = match.captured(1);
        m_status = match.captured(2);
        QString timestamp = match.captured(3);
        m_method = match.captured(4);
        m_username = match.captured(5);
        m_companyname = match.captured(6);

        m_timestamp = QDateTime::fromString(timestamp, "dd.MM.yyyy hh:mm:ss");
        if (m_timestamp.isValid()) {
            retVal = true;
        } else {
            m_timestamp = QDateTime();
            m_errorString = QStringLiteral("Error converting Timestamp value: %1").arg(timestamp);
        }
        qsizetype posStart = m_username.indexOf('(');
        qsizetype posEnd = m_username.indexOf(')', posStart + 1) - posStart - 1;
        m_role = m_username.sliced(posStart + 1, posEnd).trimmed();
        m_username.resize(posStart -1);

        // parsing Attributes && IpAddress
        m_attributes = line.mid(m_header.length());
        posStart = m_attributes.lastIndexOf(m_delimiterChar);
        QString ipaddresses = m_attributes.mid(posStart + 1);
        analizeIPAdresses(ipaddresses);
        m_attributes.resize(posStart);
        removeQuote(m_attributes, m_quoteChar);

        // parse m_attributes
        if (!parseAttributesDetails()) {
            m_username1.clear();
        }
    }
    return retVal;
}

void
CAuditTrailParser::convertData(mms::dataItem_t &data)
{
    data[phStatus] = m_status;
    data[phTimestamp] = m_timestamp;
    data[phMethod] = m_method;
    data[phUsername] = m_username;
    data[phRole] = m_role;
    data[phCompanyname] = m_companyname;
    data[phAttributes] = m_attributes;
    data[phUsername1] = m_username1;
    data[phInternalip] = m_internalip;
    data[phExternalip] = m_externalip;
}

QString
CAuditTrailParser::insertString() const
{
    return audittrail::insertData;
}

void
CAuditTrailParser::getParsedData(QString &status,
                                QDateTime &timestamp,
                                QString &method,
                                QString &username,
                                QString &role,
                                QString &companyname,
                                QString &attributes,
                                QString &username1,
                                QString &internalip,
                                QString &externalip)
{
    status = m_status;
    timestamp = m_timestamp;
    method = m_method;
    username = m_username;
    role = m_role;
    companyname = m_companyname;
    attributes = m_attributes;
    username1 = m_username1;
    internalip = m_internalip;
    externalip = m_externalip;
}

QString
CAuditTrailParser::createTable() const
{
    return audittrail::createTable;
}
