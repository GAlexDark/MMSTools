/****************************************************************************
*
* Class for parsing the MMS Audit Trail logs.
* Copyright (C) 2025  Oleksii Gaienko
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

#ifndef CAUDITTRAILPARSERBASE_H
#define CAUDITTRAILPARSERBASE_H

#include "CBasicParser.h"
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class CAuditTrailParserBase : public CBasicParser
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
    bool parsePersonDataDetails();
    bool parseLoadAuditTrail();
    bool parseAttributesDetails();

    QDateTime   m_timestamp;

    QString     m_header;
    QString     m_status;
    QString     m_method;
    QString     m_username;
    QString     m_role;
    QString     m_companyname;
    QString     m_username1;
    QString     m_attributes;

    char        m_delimiterChar = 0;
    char        m_quoteChar = 0;
};

#endif // CAUDITTRAILPARSERBASE_H
