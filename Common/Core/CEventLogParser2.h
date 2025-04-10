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

#ifndef CEVENTLOGPARSER2_H
#define CEVENTLOGPARSER2_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

#include "CEventLogParserBase.h"

class CEventLogParser2: public CEventLogParserBase
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "eventlog")
    Q_CLASSINFO("columns", "\"Ім'я користувача\",\"Відмітка часу\",\"Session ID\",\"ID запиту\",\"Тип\",\"Деталі\"|\"Username\",\"Timestamp\",\"Session ID\",\"Request ID\",\"Type\",\"Details\"")
    Q_CLASSINFO("ID", "4")
    Q_CLASSINFO("quoteChar", "\"")
    Q_CLASSINFO("delimiterChar", ",")
    Q_CLASSINFO("eolChars", "\r\n")

public:
    Q_INVOKABLE explicit CEventLogParser2(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(QMap<QString, QVariant> &data) override;
    QString insertString() const override;
#ifdef QT_DEBUG
    void getParsedData(QString &username,
                       QString &timestampISO8601,
                       QString &sessionID,
                       QString &requestID,
                       QString &type,
                       QString &details,
                       QString &username1,
                       QString &authType,
                       QString &externalIP,
                       QString &internalIP,
                       QDateTime &timestampTZ) const;
#endif
    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Event Log (CR-003)"); } // Don't use the 'const' because translation does not work.
    mms::ffs_t fileFieldsSeparationInfo() const override { return { m_delimiterChar, m_quoteChar, m_eolChars }; }

private:
    bool parseUserFailedLogonDetails() override;

    QString     m_sessionID;
};

Q_DECLARE_METATYPE(CEventLogParser2 *);

using pEventLogParser2 = CEventLogParser2 *;

#endif // CEVENTLOGPARSER2_H
