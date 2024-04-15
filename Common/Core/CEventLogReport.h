#ifndef CEVENTLOGREPORT_H
#define CEVENTLOGREPORT_H

#include "CBasicReport.h"

class CEventLogReport : public CBasicReport
{
    Q_OBJECT
    Q_CLASSINFO("ID", "1")
public:
    Q_INVOKABLE CEventLogReport();
    bool generateReport(const QString &arguments) override;
    QString visibleReportName() override { return QObject::tr("Event Log"); }
};

Q_DECLARE_METATYPE(CEventLogReport *);

typedef CEventLogReport *pEventLogReport;

#endif // CEVENTLOGREPORT_H
