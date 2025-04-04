/****************************************************************************
*
* The Base class for the MMS log parsers
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

#ifndef CBASICPARSER_H
#define CBASICPARSER_H

#include <QObject>
#include <QString>
#include "MMSTypes.h"

class CBasicParser : public QObject
{
public:
    explicit CBasicParser(QObject *parent = nullptr);
    void init(const QString &internalIpFirstOctet);
    bool checkHeader(const QString &line);
    const QString& errorString() const { return m_errorString; }

    virtual bool parse(const QString& line) = 0;
    virtual void convertData(QMap<QString, QVariant> &data) = 0;
    virtual QString insertString() const = 0;
    virtual QString createTable() const = 0;
    virtual QString visibleLogName() = 0;

    QString tableName() const;
    quint16 parserID() const;
    virtual mms::ffs_t fileFieldsSeparationInfo() const = 0;

protected:
    void removeQuote(QString &data) const;
    void analizeIPAdresses();
    void setErrorString(const QString &errorString) { m_errorString = errorString; }
    void clearErrorString() { m_errorString.clear(); }
    void initFfs(QByteArray &eolChars, char &quoteChar, char &delimiterChar);
    void setQuoteChar(char quoteChar) { m_quoteChar = quoteChar; }

    QString     m_ipaddresses;
    QString     m_internalip;
    QString     m_externalip;
    QByteArray  m_eolChars;

private:
    QString     m_internalIpFirstOctet;
    QString     m_errorString;
    char        m_quoteChar = 0;

    QStringList m_externalIpList;
    QStringList m_internalIpList;
    QStringList m_ipAddressList;
};

using pBasicParser = CBasicParser *;

#endif // CBASICPARSER_H
