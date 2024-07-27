/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CSystemLogParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for the MMS System logs parsing.
*
****************************************************************************/

#ifndef CSYSTEMLOGPARSER_H
#define CSYSTEMLOGPARSER_H

#include <QString>
#include <QDateTime>

#include "CBasicParser.h"

class CSystemLogParser: public CBasicParser
{
    Q_OBJECT
    Q_CLASSINFO("tablename", "systemlog")
    Q_CLASSINFO("columns", "Severity;Logdate;Message;User")
    Q_CLASSINFO("ID", "3")
    Q_CLASSINFO("quoteChar", "\"")
    Q_CLASSINFO("delimiterChar", ";")
    Q_CLASSINFO("eolChars", "\n")

public:
    Q_INVOKABLE explicit CSystemLogParser(QObject *parent = nullptr);
    bool parse(const QString& line) override;
    void convertData(mms::dataItem_t &data) override;
    bool checkHeader(const QString &line) override;
    QString insertString() const override;
    void getParsedData(QString &severity,
                       QDateTime &timestamp,
                       QString &message,
                       QString &username,
                       QString &username1,
                       QString &role,
                       QString &companyname) const;

    QString createTable() const override;
    QString visibleLogName() override { return QObject::tr("System Log"); } // Don't use the 'const' because translation does not work.
    mms::ffs_t fileFieldsSeparationInfo() const override { return { m_delimiterChar, m_quoteChar, m_eolChars }; }

private:
    bool parseMessage();

    QDateTime   m_timestamp;

    QString     m_severity;
    QString     m_message;
    QString     m_username;
    QString     m_username1;
    QString     m_role;
    QString     m_companyname;

    QString     m_type;

    char        m_delimiterChar;
    char        m_quoteChar = 0;
};

Q_DECLARE_METATYPE(CSystemLogParser *);

using pSystemLogParser = CSystemLogParser *;

#endif // CSYSTEMLOGPARSER_H
