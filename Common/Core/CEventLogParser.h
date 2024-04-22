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

#include <QDateTime>

#include "CBasicParser.h"

class CEventLogParser: public CBasicParser
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "eventlog")
    Q_CLASSINFO("ID", "1")
public:
    Q_INVOKABLE CEventLogParser(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(mms::dataItem_t &data) override;
    QString insertString() const override;
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

    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Event Log"); } // Don't use the 'const' because translation does not work.

private:
    struct userSuccessLogonDetails_t
    {
        QString details;
        QString username;
        QString authType;
        QString internalIp;
        QString externalIp;
    } m_prevValueUSLD;

    struct userFailedLogonDetails_t
    {
        QString details;
        QString authType;
        QString internalIp;
        QString externalIp;
    } m_prevValueUFLD;

    QDateTime   m_timestamp,
                m_timestamptz;
    QString     m_header,
                m_details,
                m_timestampISO8601,
                m_format;
    QString     m_username,
                m_username1,
                m_authType,
                m_requestID,
                m_type;

    bool parseUserSuccessLogonDetails();
    bool parseUserFailedLogonDetails();
    bool parseUserLogonDetails();
};

Q_DECLARE_METATYPE(CEventLogParser *);

typedef CEventLogParser *pEventLogParser;

#endif // CEVENTLOGPARSER_H
