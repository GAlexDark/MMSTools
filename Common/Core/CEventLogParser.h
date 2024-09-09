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

#ifndef CEVENTLOGPARSER_H
#define CEVENTLOGPARSER_H

#include <QDateTime>

#include "CBasicParser.h"

class CEventLogParser: public CBasicParser
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "eventlog")
    Q_CLASSINFO("columns", "\"Ім'я користувача\",\"Відмітка часу\",\"ID запиту\",\"Тип\",\"Деталі\"|\"Username\",\"Timestamp\",\"Request ID\",\"Type\",\"Details\"")
    Q_CLASSINFO("ID", "1")
    Q_CLASSINFO("quoteChar", "\"")
    Q_CLASSINFO("delimiterChar", ",")
    Q_CLASSINFO("eolChars", "\r\n")

public:
    Q_INVOKABLE explicit CEventLogParser(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(mms::dataItem_t &data) override;
    bool checkHeader(const QString &line) override;
    QString insertString() const override;
    void getParsedData(QString &username,
                       QString &timestampISO8601,
                       QString &requestID,
                       QString &type,
                       QString &details,
                       QString &username1,
                       QString &authType,
                       QString &externalIP,
                       QString &internalIP,
                       QDateTime &timestampTZ) const;

    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Event Log"); } // Don't use the 'const' because translation does not work.
    mms::ffs_t fileFieldsSeparationInfo() const override { return { m_delimiterChar, m_quoteChar, m_eolChars }; }

private:
    bool parseUserSuccessLogonDetails();
    bool parseUserFailedLogonDetails();
    bool parseUserLogonDetails();

    struct userSuccessLogonDetails_t
    {
        QString details;
        QString username;
        QString authType;
        QString internalIp;
        QString externalIp;
    };
    userSuccessLogonDetails_t m_prevValueUSLD;

    struct userFailedLogonDetails_t
    {
        QString details;
        QString authType;
        QString internalIp;
        QString externalIp;
    };
    userFailedLogonDetails_t m_prevValueUFLD;

    QDateTime   m_timestamp;
    QDateTime   m_timestamptz;
    QString     m_header;
    QString     m_details;
    QString     m_timestampISO8601;
    QString     m_format;
    QString     m_username;
    QString     m_username1;
    QString     m_authType;
    QString     m_requestID;
    QString     m_type;

    char        m_delimiterChar;
    char        m_quoteChar = 0;
};

Q_DECLARE_METATYPE(CEventLogParser *);

using pEventLogParser = CEventLogParser *;

#endif // CEVENTLOGPARSER_H
