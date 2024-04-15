#ifndef CAUDITTRAILREPORT_H
#define CAUDITTRAILREPORT_H

#include "CBasicReport.h"

class CAuditTrailReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "2")
public:
    Q_INVOKABLE CAuditTrailReport();
    bool generateReport(const QString &arguments) override;
    QString visibleReportName() override { return QObject::tr("Audit Trail Log"); }
};

Q_DECLARE_METATYPE(CAuditTrailReport *);

typedef CAuditTrailReport *pAuditTrailReport;

#endif // CAUDITTRAILREPORT_H
