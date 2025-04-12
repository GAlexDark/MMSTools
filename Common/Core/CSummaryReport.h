/****************************************************************************
*
* The class for generating the consolidated MMS log reports.
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

#ifndef CSUMMARYREPORT_H
#define CSUMMARYREPORT_H

#include "CBasicReport.h"

class CSummaryReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "1000")
    Q_CLASSINFO("source", "eventlog|audittraillog|systemlog")
public:
    Q_INVOKABLE explicit CSummaryReport(QObject *parent = nullptr);
    bool generateReport() final;
    QString visibleReportName() final { return QObject::tr("(Experimental) Summary report"); }
    QString selectString() const final;
private:
    void addReportHeader(QXlsx::Document *xlsxReport, int row);
    void addReportRow(QXlsx::Document *xlsxReport, int row, int multipartRowCount, const QXlsx::Format &dateFormat);
};

Q_DECLARE_METATYPE(CSummaryReport *);

using pSummaryReport = CSummaryReport *;

#endif // CSUMMARYREPORT_H
