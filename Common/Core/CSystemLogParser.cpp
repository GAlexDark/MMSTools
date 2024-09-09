/****************************************************************************
*
* Class for parsing the MMS System logs.
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

#include "CSystemLogParser.h"
#include <QRegularExpression>

#include "DBStrings.h"
#include "elcUtils.h"

const QRegularExpression reSystemLogHeader(QLatin1String("^(.*?);(.*?);(.*?);(.*?)"));
const QRegularExpression reUserData1(QLatin1String("^User\\s\"(.*?)\".*?\"(.*?)\".*?\"(.*?)\""));
const QRegularExpression reUserData2(QLatin1String("^User\\s\"(.*?)\""));

const QString nullValue(QLatin1String("null"));
const QString user(QLatin1String("User "));
const QString usingRole(QLatin1String("Using role"));
const QString loginSuccess(QLatin1String("User login - successful"));
const QString email(QLatin1String("Email"));
const QString received(QLatin1String("received"));
const QString processed(QLatin1String("processed"));
const QString emailReceived(QLatin1String("Email - received"));
const QString emailProcessed(QLatin1String("Email - processed"));
const QString fileProcessing(QLatin1String("file processing"));
const QString started(QLatin1String("started"));
const QString finished(QLatin1String("finished"));
const QString fileProcessingStarted(QLatin1String("File processing - started"));
const QString fileProcessingFinished(QLatin1String("File processing - finished"));

const QString gate(QLatin1String("Gate"));
const QString gateAutomaticallyOpened(QLatin1String("Gate - automatically opened"));
const QString gateAutomaticallyClosed(QLatin1String("Gate - automatically closed"));
const QString gateReopened(QLatin1String("Gate - reopened"));
const QString gateOpened(QLatin1String("Gate - opened"));
const QString gateClosed(QLatin1String("Gate - closed"));
const QString automaticallyOpened(QLatin1String("automatically opened"));
const QString automaticallyClosed(QLatin1String("automatically closed"));
const QString opened(QLatin1String("opened"));
const QString reopened(QLatin1String("reopened"));
const QString closed(QLatin1String("closed"));

CSystemLogParser::CSystemLogParser(QObject *parent)
    : CBasicParser{parent}
{
    clearErrorString();
    initFfs(m_eolChars, m_quoteChar, m_delimiterChar);
    setQuoteChar(m_quoteChar);
}

bool
CSystemLogParser::parseUserData()
{
    bool retVal = m_message.startsWith(user);
    if (retVal) {
        QRegularExpressionMatch match1 = reUserData1.match(m_message);
        if (match1.hasMatch()) {
            m_username1 = match1.captured(1);
            m_role = match1.captured(2);
            m_companyname = match1.captured(3);
            m_type = usingRole;
        } else {
            QRegularExpressionMatch match2 = reUserData2.match(m_message);
            if (match2.hasMatch()) {
                m_username1 = match2.captured(1);
                m_role.clear();
                m_companyname.clear();
                m_type = loginSuccess;
            } else {
                retVal = false;
            }
        }
    }
    return retVal;
}

bool
CSystemLogParser::parseEmailMessages()
{
    bool retVal = m_message.startsWith(email);
    if (retVal) {
        if (m_message.indexOf(received) != -1) {
            m_username1.clear();
            m_role.clear();
            m_companyname.clear();
            m_type = emailReceived;
        } else {
            if (m_message.indexOf(processed) != -1) {
                m_username1.clear();
                m_role.clear();
                m_companyname.clear();
                m_type = emailProcessed;
            } else {
                retVal = false;
            }
        }
    }
    return retVal;
}

bool
CSystemLogParser::parseFileInfo()
{
    bool retVal = m_message.startsWith(fileProcessing);
    if (retVal) {
        if (m_message.indexOf(started) != -1) {
            m_username1.clear();
            m_role.clear();
            m_companyname.clear();
            m_type = fileProcessingStarted;
        } else {
            if (m_message.indexOf(finished) != -1) {
                m_username1.clear();
                m_role.clear();
                m_companyname.clear();
                m_type = fileProcessingFinished;
            } else {
                retVal = false;
            }
        }
    }
    return retVal;
}

bool
CSystemLogParser::parseGateInfo()
{
    bool retVal = m_message.startsWith(gate);
    if (retVal) {
        if (m_message.indexOf(automaticallyOpened) != -1) {
            m_username1.clear();
            m_role.clear();
            m_companyname.clear();
            m_type = gateAutomaticallyOpened;
        } else {
            if (m_message.indexOf(automaticallyClosed) != -1) {
                m_username1.clear();
                m_role.clear();
                m_companyname.clear();
                m_type = gateAutomaticallyClosed;
            } else {
                if (m_message.indexOf(reopened) != -1) {
                    m_username1.clear();
                    m_role.clear();
                    m_companyname.clear();
                    m_type = gateReopened;
                } else {
                    if (m_message.indexOf(opened) != -1) {
                        m_username1.clear();
                        m_role.clear();
                        m_companyname.clear();
                        m_type = gateOpened;
                    } else {
                        if (m_message.indexOf(closed) != -1) {
                            m_username1.clear();
                            m_role.clear();
                            m_companyname.clear();
                            m_type = gateClosed;
                        } else {
                            retVal = false;
                        }
                    }
                }
            }
        }
    }
    return retVal;
}

bool
CSystemLogParser::parseMessage()
{
    bool retVal = parseUserData();
    if (!retVal) {
        retVal = parseEmailMessages();
        if (!retVal) {
            retVal = parseFileInfo();
            if (!retVal) {
                retVal = parseGateInfo();
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
