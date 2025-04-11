#ifndef CAUDITTRAILPARSERBASE_H
#define CAUDITTRAILPARSERBASE_H

#include "CBasicParser.h"
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class CAuditTrailParserBase : public CBasicParser
{
    Q_OBJECT
public:
    using CBasicParser::CBasicParser;
    virtual bool parse(const QString& line) override = 0;
    virtual void convertData(QMap<QString, QVariant> &data) override = 0;
    virtual QString insertString() const override = 0;
    virtual QString createTable() const override = 0;
    virtual QString visibleLogName() override = 0;
    virtual mms::ffs_t fileFieldsSeparationInfo() const override = 0;

protected:
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
    QString     m_username1;
    QString     m_attributes;

    char        m_delimiterChar;
    char        m_quoteChar = 0;
};

#endif // CAUDITTRAILPARSERBASE_H
