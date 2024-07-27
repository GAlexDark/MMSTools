/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSystemLogParser.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for the MMS System logs parsing.
*
****************************************************************************/

#include "CSystemLogParser.h"
#include <QRegularExpression>

#include "DBStrings.h"
#include "elcUtils.h"

const QRegularExpression reSystemLogHeader(QLatin1String("^(.*?);(.*?);(.*?);(.*?)"));
const QRegularExpression reUserData1(QLatin1String("^User\\s\"(.*?)\".*?\"(.*?)\".*?\"(.*?)\""));
const QRegularExpression reUserData2(QLatin1String("^User\\s\"(.*?)\""));

const QString nullValue(QLatin1String("null"));
const QString user(QLatin1String("User "));

CSystemLogParser::CSystemLogParser(QObject *parent)
    : CBasicParser{parent}
{
    clearErrorString();
    initFfs(m_eolChars, m_quoteChar, m_delimiterChar);
    setQuoteChar(m_quoteChar);
}

bool
CSystemLogParser::parseMessage()
{
    bool retVal = m_message.startsWith(user);
    if (retVal) {
        QRegularExpressionMatch match1 = reUserData1.match(m_message);
        if (match1.hasMatch()) {
            m_username1 = match1.captured(1);
            m_role = match1.captured(2);
            m_companyname = match1.captured(3);
            m_type = QStringLiteral("Using role");
        } else {
            QRegularExpressionMatch match2 = reUserData2.match(m_message);
            if (match2.hasMatch()) {
                m_username1 = match2.captured(1);
                m_role.clear();
                m_companyname.clear();
                m_type = QStringLiteral("User login - successful");
            } else {
                retVal = false;
            }
        }
    }
    return retVal;
}


bool
CSystemLogParser::parse(const QString& line)
{
    bool retVal = false;
    QString buf(line);
    if (buf.indexOf("&quot;") != -1) {
        buf.replace(QLatin1String("&quot;"), QLatin1String("\""));
    }
    QRegularExpressionMatch match = reSystemLogHeader.match(buf);
    if (match.hasMatch()) {
        m_severity = match.captured(1);
        QString timestamp = match.captured(2);
        m_timestamp = QDateTime::fromString(timestamp, "dd.MM.yyyy hh:mm:ss");
        if (m_timestamp.isValid()) {
            retVal = true;
        } else {
            m_timestamp = QDateTime();
            setErrorString(QStringLiteral("Error converting Timestamp value: %1").arg(timestamp));
        }
        m_message = match.captured(3);
        removeQuote(m_message);
        if (!parseMessage()) {
            m_username1.clear();
            m_role.clear();
            m_companyname.clear();
            m_type.clear();
        }
        m_username = match.captured(4);
    } else {
        setErrorString(QStringLiteral("Wrong header.\nDetails: %1").arg(line));
    }
    return retVal;
}

void
CSystemLogParser::convertData(mms::dataItem_t &data)
{
    data[phSeverity] = m_severity;
    data[phTimestamp] = m_timestamp;
    data[phMessage] = m_message;
    data[phUsername] = m_username;
    data[phUsername1] = m_username1;
    data[phRole] = m_role;
    data[phCompanyname] = m_companyname;
    data[phType] = m_type;
}

bool
CSystemLogParser::checkHeader(const QString &line)
{
    QString columns;
    bool retVal = elcUtils::getMetaClassInfo(this, "columns", columns);
    Q_ASSERT(retVal);
    return QString::compare(columns, line.trimmed(), Qt::CaseInsensitive) == 0 ? true : false;
}

QString
CSystemLogParser::insertString() const
{
    return systemlog::insertData;
}

void
CSystemLogParser::getParsedData(QString &severity,
                                QDateTime &timestamp,
                                QString &message,
                                QString &username,
                                QString &username1,
                                QString &role,
                                QString &companyname) const
{
    severity = m_severity;
    timestamp = m_timestamp;
    message = m_message;
    username = m_username;
    username1 = m_username1;
    role = m_role;
    companyname = m_companyname;
}

QString
CSystemLogParser::createTable() const
{
    return systemlog::createTable;
}
