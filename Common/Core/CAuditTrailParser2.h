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

#ifndef CAUDITTRAILPARSER2_H
#define CAUDITTRAILPARSER2_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

#include "CAuditTrailParserBase.h"

class CAuditTrailParser2: public CAuditTrailParserBase
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "audittraillog")
    Q_CLASSINFO("columns", "Succeeded;Date;Method;Username;Companyname;Attributes;IpAddress;SessionId")
    Q_CLASSINFO("ID", "5")
    Q_CLASSINFO("quoteChar", "\"")
    Q_CLASSINFO("delimiterChar", ";")
    Q_CLASSINFO("eolChars", "\n")

public:
    Q_INVOKABLE explicit CAuditTrailParser2(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(QMap<QString, QVariant> &data) override;
    QString insertString() const override;
    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Audit Trail Log (CR-003)"); } // Don't use the 'const' because translation does not work.
    //mms::ffs_t fileFieldsSeparationInfo() const override { return { m_delimiterChar, m_quoteChar, m_eolChars }; }
#ifdef QT_DEBUG
    void getParsedData(QString &status,
                       QDateTime &timestamp,
                       QString &method,
                       QString &username,
                       QString &role,
                       QString &companyname,
                       QString &attributes,
                       QString &username1,
                       QString &internalip,
                       QString &externalip,
                       QString &sessionId) const;
#endif

private:
    QString     m_sessionId;
};

Q_DECLARE_METATYPE(CAuditTrailParser2 *);

using pAuditTrailParser2 = CAuditTrailParser2 *;

#endif // CAUDITTRAILPARSER2_H
