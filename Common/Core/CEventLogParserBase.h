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

#ifndef CEVENTLOGPARSERBASE_H
#define CEVENTLOGPARSERBASE_H

#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QMap>

#include "CBasicParser.h"

class CEventLogParserBase: public CBasicParser
{
    Q_OBJECT

public:
    using CBasicParser::CBasicParser;
    bool parse(const QString& line) override = 0;
    void convertData(QMap<QString, QVariant> &data) override = 0;
    QString insertString() const override = 0;
    QString createTable() const override = 0;
    QString visibleLogName() override = 0;
    mms::ffs_t fileFieldsSeparationInfo() const final { return { m_delimiterChar, m_quoteChar, m_eolChars }; }

protected:
    virtual bool parseUserFailedLogonDetails() = 0;
    bool parseUserSuccessLogonDetails();
    bool userSuccessLogonDetails();
    bool userFailedLogonDetails();
    bool parseUserLogonDetails();

    QString m_username;
    QString m_timestampISO8601;
    QString m_requestID;
    QString m_type;
    QString m_details;
    QString m_username1;
    QString m_authType;
    QDateTime m_timestamp;
    QDateTime m_timestamptz;
    QString m_header;

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

    char m_delimiterChar = 0;
    char m_quoteChar = 0;
};

#endif // CEVENTLOGPARSERBASE_H
