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
#include "elcUtils.h"

class CBasicParser : public QObject
{
public:
    explicit CBasicParser(QObject *parent = nullptr);
    void init(const QString &internalIpFirstOctet);
    QString errorString() const { return m_errorString; }

    virtual bool parse(const QString& line) = 0;
    virtual void convertData(mms::dataItem_t &data) = 0;
    virtual bool checkHeader(const QString &line) = 0;
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

    QString     m_ipaddresses;
    QString     m_internalip;
    QString     m_externalip;
    char        m_quoteChar = 0;
    QByteArray  m_eolChars;

private:
    QString     m_internalIpFirstOctet;
    QString     m_errorString;

};

typedef CBasicParser *pBasicParser;

#endif // CBASICPARSER_H
