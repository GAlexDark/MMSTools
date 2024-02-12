/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CEventLogParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for MMS Event logs parsing.
*
****************************************************************************/

#ifndef CEVENTLOGPARSER_H
#define CEVENTLOGPARSER_H

#include <QObject>
#include <QDateTime>

class CEventLogParser
{
public:
    void init(const QString &internalIpFirstOctet);
    bool parse(const QString& line);
    QString errorString() const { return m_errorString; }

    void getParsedData(QString &username,
                       QString &timestampISO8601,
                       QString &requestID,
                       QString &type,
                       QString &details,
                       QString &username1,
                       QString &authType,
                       QString &externalIP,
                       QString &internalIP,
                       QDateTime &timestampTZ);

private:
    QString     m_internalIpFirstOctet;
    QString     m_errorString;

    QDateTime   m_timestamp,
                m_timestamptz;
    QString     m_header,
                m_details,
                m_timestampISO8601,
                m_format;
    QString     m_username,
                m_username1,
                m_authType,
                m_externalip,
                m_internalip,
                m_requestID,
                m_type;

    void removeQuote(QString &data, QChar quoteChar);
    void parseHeaderString(QChar quoteChar);

    void analizeIPAdresses(const QString &ipaddresses);
    bool parseUserSuccessLogonDetails();
    bool parseUserFailedLogonDetails();
    bool parseUserLogonDetails();

};

#endif // CEVENTLOGPARSER_H
