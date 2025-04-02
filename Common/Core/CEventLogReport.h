/****************************************************************************
*
* The class for generating the MMS Event log reports.
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

#ifndef CEVENTLOGREPORT_H
#define CEVENTLOGREPORT_H

#include "CBasicReport.h"

class CEventLogReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "1")
    Q_CLASSINFO("source", "eventlog")
public:
    Q_INVOKABLE explicit CEventLogReport(QObject *parent = nullptr);
    bool generateReport() override;
    QString visibleReportName() override { return QObject::tr("Event Log"); }
    QString selectString() const override;
private:
    void addReportHeader(QXlsx::Document *xlsxReport, int row);
    void addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat, bool &isDataTooLong);
};

Q_DECLARE_METATYPE(CEventLogReport *);

using pEventLogReport = CEventLogReport *;

#endif // CEVENTLOGREPORT_H
