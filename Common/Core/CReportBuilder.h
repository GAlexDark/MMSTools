/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CReportBuilder.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The classes for creating the report from MMS Event Logs.
*
****************************************************************************/


#ifndef CREPORTBUILDER_H
#define CREPORTBUILDER_H

#include <QThread>
#include <QString>
#include <QStringList>

#include "CBasicDatabase.h"
#include "CBasicReport.h"

class CReportBuilder
{
public:
    explicit CReportBuilder();
    virtual ~CReportBuilder();
    bool init(quint16 logID, const QString &dbFileName, const QString &reportName,
              const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList);
    QString errorString() const { return m_errorString; }
    bool generateReport();

private:
    CReportBuilder(const CReportBuilder&) = delete;
    CReportBuilder& operator=(CReportBuilder&) = delete;

    pBasicDatabase   m_db = nullptr;
    pBasicReport     m_report = nullptr; // don't use the 'detele' operator, the ReportManager manage resources
    QString         m_errorString;
    QStringList     m_excludedUsernamesList;
    QStringList     m_includedUsernamesList;
};

//-----------------------------------------------------------------

class CSVThreadReportBuilder: public QThread
{
public:
    explicit CSVThreadReportBuilder(QObject *parent = nullptr);
    bool init(quint16 logID, const QString &dbFileName, const QString &reportName,
              const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList);
    void run();
    QString errorString() const { return m_errorString; }
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    CReportBuilder  m_builder;
    QString         m_errorString;
    bool            m_retVal = false;

};


#endif // CREPORTBUILDER_H
