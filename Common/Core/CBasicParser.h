/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CBasicParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the parsers
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
    CBasicParser();
    virtual ~CBasicParser() = default;
    void init(const QString &internalIpFirstOctet);
    QString errorString() const { return m_errorString; }
    virtual QString insertString() const = 0;
    virtual bool parse(const QString& line) = 0;
    virtual void convertData(mms::dataItem_t &data) = 0;

    virtual QString createTable() const =0;
    virtual QString visibleLogName() =0;
    QString tableName() const;
    quint16 parserID() const;
    mms::ffs_t fileFieldsSeparationInfo();

protected:
    void removeQuote(QString &data, char quoteChar);
    void analizeIPAdresses();

    QString     m_internalIpFirstOctet;
    QString     m_ipaddresses,
                m_internalip,
                m_externalip;

    char        m_delimiterChar;
    char        m_quoteChar;
    QByteArray  m_eolChars;

    QString     m_errorString;
};

typedef CBasicParser *pBasicParser;

#endif // CBASICPARSER_H
