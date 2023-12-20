#ifndef CREPORTBUILDER_H
#define CREPORTBUILDER_H

#include <QThread>
#include "CBasicDBClass.h"

class CReportBuilder
{
public:
    bool init(const QString &dbFileName, const QString &reportName);
    QString errorString() const { return m_errorString; }
    bool generateReport();

private:
    CBasicDBClass   m_db;
    QString         m_errorString;
    QString         m_reportFileName;
};

//-----------------------------------------------------------------

class CSVThreadReportBuilder: public QThread
{
public:
    bool init(const QString &dbFileName, const QString &reportName);
    void run();
    QString errorString() const { return m_errorString; }
    bool getStatus() const { return m_retVal; }

private:
    CReportBuilder  m_builser;
    QString         m_errorString;
    bool            m_retVal;
};


#endif // CREPORTBUILDER_H
