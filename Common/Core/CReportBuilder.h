#ifndef CREPORTBUILDER_H
#define CREPORTBUILDER_H

#include <QThread>
#include <QString>
#include <QStringList>

#include "CBasicDatabase.h"

class CReportBuilder
{
public:
    explicit CReportBuilder();
    virtual ~CReportBuilder();
    bool init(const QString &dbFileName, const QString &reportName, QStringList *excludedUsernamesList, QStringList *includedUsernamesList);
    QString errorString() const { return m_errorString; }
    bool generateReport();

private:
    CReportBuilder(const CReportBuilder&) = delete;
    CReportBuilder& operator=(CReportBuilder&) = delete;

    CBasicDatabase   *m_db;
    QString         m_errorString;
    QString         m_reportFileName;
    QStringList     m_excludedUsernamesList,
                    m_includedUsernamesList;
};

//-----------------------------------------------------------------

class CSVThreadReportBuilder: public QThread
{
public:
    explicit CSVThreadReportBuilder();
    bool init(const QString &dbFileName, const QString &reportName, QStringList *excludedUsernamesList, QStringList *includedUsernamesList);
    void run();
    QString errorString() const { return m_errorString; }
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    CReportBuilder  m_builser;
    QString         m_errorString;
    bool            m_retVal;

};


#endif // CREPORTBUILDER_H
