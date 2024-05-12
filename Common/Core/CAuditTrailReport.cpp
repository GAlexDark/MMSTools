/****************************************************************************
 *
 *  Copyright (c) Oleksii Gaienko, 2024
 *  Contact: galexsoftware@gmail.com
 *
 *  Event Log Conversion Utility
 *  Common module
 *
 *  Module name: CAuditTrailReport.cpp
 *  Author(s): Oleksii Gaienko
 *  Reviewer(s):
 *
 *  Abstract:
 *     The class for build MMS Audit Trail logs report.
 *
 ****************************************************************************/

#include "CAuditTrailReport.h"
#include "DBStrings.h"

CAuditTrailReport::CAuditTrailReport(QObject *parent) : CBasicReport{parent} {
  clearErrorString();
}

bool CAuditTrailReport::generateReport(const QString &arguments) {
  bool retVal = m_db->exec(audittrail::selectData.arg(arguments));
  if (retVal) {
    // Set datetime format
    QXlsx::Format dateFormat;
    setDateTimeFormat(dateFormat);

    int row = 1;
    int colRowNumber = 1;
    int colTimestamp = 2;
    int colUsername = 3;
    int colRole = 4;
    int colCompanyname = 5;
    int colMethod = 6;
    int colStatus = 7;
    int colAttributes = 8;
    int colInternalIP = 9;

    QXlsx::Document xlsxReport;
    // Add header
    QVariant writeValue = QStringLiteral("№");
    xlsxReport.write(row, colRowNumber, writeValue);

    writeValue = QStringLiteral("Відмітка часу (за Київським часом)");
    xlsxReport.write(row, colTimestamp, writeValue);
    writeValue = QStringLiteral("Ім'я користувача");
    xlsxReport.write(row, colUsername, writeValue);
    writeValue = QStringLiteral("Роль");
    xlsxReport.write(row, colRole, writeValue);
    writeValue = QStringLiteral("Компанія");
    xlsxReport.write(row, colCompanyname, writeValue);
    writeValue = QStringLiteral("Метод");
    xlsxReport.write(row, colMethod, writeValue);
    writeValue = QStringLiteral("Статус");
    xlsxReport.write(row, colStatus, writeValue);
    writeValue = QStringLiteral("Атрибути");
    xlsxReport.write(row, colAttributes, writeValue);
    writeValue = QStringLiteral("Внутрішній IP");
    xlsxReport.write(row, colInternalIP, writeValue);
    ++row;

    while (m_db->isNext()) {
      xlsxReport.write(row, colRowNumber, row - 1);

      writeValue = m_db->geValue("timestamp").toDateTime();
      xlsxReport.write(row, colTimestamp, writeValue, dateFormat);

      setReportDataItem(&xlsxReport, "username", colUsername, row);
      setReportDataItem(&xlsxReport, "role", colRole, row);
      setReportDataItem(&xlsxReport, "companyname", colCompanyname, row);
      setReportDataItem(&xlsxReport, "method", colMethod, row);
      setReportDataItem(&xlsxReport, "status", colStatus, row);
      setReportDataItem(&xlsxReport, "attributes", colAttributes, row);
      setReportDataItem(&xlsxReport, "internalip", colInternalIP, row);

      ++row;
    } // while

    retVal = xlsxReport.saveAs(m_reportFileName);
    if (!retVal) {
      setErrorString(QStringLiteral("Error save report file"));
    }
  } else {
    setErrorString(m_db->errorString());
  }

  return retVal;
}

void CAuditTrailReport::setReportDataItem(QXlsx::Document *report,
                                          const int dbFieldIndex,
                                          const int reportFieldIndex,
                                          const int row) {
  CBasicReport::setReportDataItem(report, m_db, dbFieldIndex, reportFieldIndex,
                                  row);
}

void CAuditTrailReport::setReportDataItem(QXlsx::Document *report,
                                          const QString &dbFieldName,
                                          const int reportFieldIndex,
                                          const int row) {
  CBasicReport::setReportDataItem(report, m_db, dbFieldName, reportFieldIndex,
                                  row);
}
