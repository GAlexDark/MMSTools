#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QObject>
#include <QDateTime>

class CSVParser
{
public:
    CSVParser();
    void parse(const QString& line);
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
    QString     m_errorString;

    void removeQuote(QString &data, QChar quoteChar);
    void parseHeaderString(QChar quoteChar);

    void analizeIPAdresses(const QString &ipaddresses, QString &externalIP, QString &internalIP);
    bool parseUserSuccessLogonDetails();
    bool parseUserFailedLogonDetails();
    bool parseUserLogonDetails();

};

#endif // CSVPARSER_H
