/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CAuditTrailParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for MMS Audit Trail logs parsing.
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
    void convertData(mms::dataItem_t &data) override;
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
};

Q_DECLARE_METATYPE(CAuditTrailParser *);

typedef CAuditTrailParser *pAuditTrailParser;

#endif // CAUDITTRAILPARSER_H
