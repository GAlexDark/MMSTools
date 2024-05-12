/****************************************************************************
 *
 *  Copyright (c) Oleksii Gaienko, 2024
 *  Contact: galexsoftware@gmail.com
 *
 *  Event Log Conversion Utility
 *  Common module
 *
 *  Module name: CAuditTrailReport.h
 *  Author(s): Oleksii Gaienko
 *  Reviewer(s):
 *
 *  Abstract:
 *     The class for build MMS Audit Trail logs report.
 *
 ****************************************************************************/

#ifndef CAUDITTRAILREPORT_H
#define CAUDITTRAILREPORT_H

#include "CBasicReport.h"

class CAuditTrailReport : public CBasicReport {
  Q_OBJECT
  Q_CLASSINFO("ID", "2")
public:
  Q_INVOKABLE explicit CAuditTrailReport(QObject *parent = nullptr);
  bool generateReport(const QString &arguments) override;
  QString visibleReportName() override {
    return QObject::tr("Audit Trail Log");
  }

private:
  inline void setReportDataItem(QXlsx::Document *report, const int dbFieldIndex,
                                const int reportFieldIndex, const int row);
  inline void setReportDataItem(QXlsx::Document *report,
                                const QString &dbFieldName,
                                const int reportFieldIndex, const int row);
};

Q_DECLARE_METATYPE(CAuditTrailReport *);

typedef CAuditTrailReport *pAuditTrailReport;

#endif // CAUDITTRAILREPORT_H
