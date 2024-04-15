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
    Q_CLASSINFO("ID", "2")
public:
    Q_INVOKABLE CAuditTrailParser();
    bool parse(const QString& line) override;
    void convertData(mms::dataItem_t &data) override;
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
                       QString &externalip);

    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("Audit Trail Log"); } // Don't use the 'const' because translation does not work.

private:
    bool parsePersonDataDetails();
    bool parseLoadAuditTrail();
    bool parseAttributesDetails();

    QDateTime   m_timestamp;

    QString     m_header,
                m_status,
                m_method,
                m_username,
                m_role,
                m_companyname,
                m_attributes,
                m_username1;

};

Q_DECLARE_METATYPE(CAuditTrailParser *);

typedef CAuditTrailParser *pAuditTrailParser;

#endif // CAUDITTRAILPARSER_H
