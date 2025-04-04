/****************************************************************************
*
* Class for parsing the MMS Audit Trail logs.
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

#ifndef CAUDITTRAILPARSER_H
#define CAUDITTRAILPARSER_H

#include <QString>
#include <QDateTime>

#include "CBasicParser.h"

class CAuditTrailParser: public CBasicParser
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "audittraillog")
    Q_CLASSINFO("columns", "Succeeded;Date;Method;Username;Companyname;Attributes;IpAddress")
    Q_CLASSINFO("ID", "2")
    Q_CLASSINFO("quoteChar", "\"")
    Q_CLASSINFO("delimiterChar", ";")
    Q_CLASSINFO("eolChars", "\n")

public:
    Q_INVOKABLE explicit CAuditTrailParser(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(QMap<QString, QVariant> &data) override;
    bool checkHeader(const QString &line) override;
    QString insertString() const override;
    void getParsedData(QString &status,
                       QDateTime &timestamp,
                       QString &method,
                       QString &username,
                       QString &role,
                       QString &companyname,
                       QString &attributes,
                       QString &username1,
                       QString &internalip,
                       QString &externalip) const;

    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Audit Trail Log"); } // Don't use the 'const' because translation does not work.
    mms::ffs_t fileFieldsSeparationInfo() const override { return { m_delimiterChar, m_quoteChar, m_eolChars }; }

private:
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
    QString     m_attributes;
    QString     m_username1;

    char        m_delimiterChar;
    char        m_quoteChar = 0;
};

Q_DECLARE_METATYPE(CAuditTrailParser *);

using pAuditTrailParser = CAuditTrailParser *;

#endif // CAUDITTRAILPARSER_H
