/****************************************************************************
*
* The classes for generating the MMS Event Log reports.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CREPORTBUILDER_H
#define CREPORTBUILDER_H

#include <QThread>
#include <QString>
#include <QStringList>
#include "CSqliteDatabase.h"
#include "CBasicReport.h"

class CReportBuilder
{
public:
    explicit CReportBuilder();
    virtual ~CReportBuilder();
    bool init(quint16 logID, const QString &dbFileName, const QString &reportName,
              const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList,
              const bool showMilliseconds);
    const QString& errorString() const { return m_errorString; }
    bool generateReport();

private:
    CReportBuilder(const CReportBuilder&) = delete;
    CReportBuilder& operator=(CReportBuilder&) = delete;
    bool configureDb(const QString &dbFileName);

    CSqliteDatabase  m_db;
    pBasicReport    m_report = nullptr; // don't use the 'detele' operator, the ReportManager manage resources
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
              const QStringList *excludedUsernamesList, const QStringList *includedUsernamesList,
              const bool showMilliseconds);
    void run() override;
    const QString& errorString() const { return m_errorString; }
    bool getStatus() const { return m_retVal; }

signals:
    void sendMessage(const QString &msg);

private:
    CReportBuilder  m_builder;
    QString         m_errorString;
    bool            m_retVal = false;
};


#endif // CREPORTBUILDER_H
