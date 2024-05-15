/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CBasicReport.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the reports
*
****************************************************************************/

#include "CBasicReport.h"
#include <QMetaObject>
#include <QMetaClassInfo>
#include "elcUtils.h"

const QString eolChar(QLatin1String("\n"));
const QString emptyChar("");
const QString vs_long(QLatin1String("VALIDATE_SCHEDULES"));
const QString vs_short(QLatin1String("V_S"));
const QString std_long(QLatin1String("SEND_TO_DAM"));
const QString std_short(QLatin1String("S_T_D"));
const QString doubleSpace(QLatin1String("  ")); //2
const QString space(QLatin1String(" "));
const QString doubleBackslash(QLatin1String("\"\""));
const QString backslash(QLatin1String("\""));

CBasicReport::CBasicReport(QObject *parent)
    : QObject{parent}
{
    m_errorString.clear();
    m_reportFileName.clear();
}

void
CBasicReport::init(pBasicDatabase db, const QString &reportName, bool showMilliseconds)
{
    m_db = db;
    Q_CHECK_PTR(m_db);
    m_reportFileName = reportName;
    m_showMilliseconds = showMilliseconds;
}

void
CBasicReport::setDateTimeFormat(QXlsx::Format &dateFormat)
{
    dateFormat.setHorizontalAlignment(QXlsx::Format::AlignRight);
    QString dateTimeFormat = m_showMilliseconds ? QLatin1String("dd.mm.yyyy hh:mm:ss.000") : QLatin1String("dd.mm.yyyy hh:mm:ss");
    dateFormat.setNumberFormat(dateTimeFormat);
}

QString
CBasicReport::checkDetails(const QString &data)
{
    QString buf = data;
    buf.replace(doubleBackslash, backslash);
    //ref: https://support.microsoft.com/en-us/office/excel-specifications-and-limits-1672b34d-7043-467e-8e27-269d656771c3
    if (buf.length() > 32767) { //this line is too long
        buf.replace(eolChar, emptyChar);
        while (buf.indexOf(doubleSpace) != -1) {
            buf.replace(doubleSpace, space);
        }
        buf.replace(vs_long, vs_short);
        buf.replace(std_long, std_short);
    }
    return buf;
}

void
CBasicReport::setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row)
{
    QVariant writeValue = m_db->geValue(dbFieldIndex);
    report->write(row, reportFieldIndex, writeValue);
}

void
CBasicReport::setReportDataItem(QXlsx::Document *report, const QString &dbFieldName, const int reportFieldIndex, const int row)
{
    QVariant writeValue = m_db->geValue(dbFieldName);
    report->write(row, reportFieldIndex, writeValue);
}

quint16
CBasicReport::reportID() const
{
    QString value;
    return elcUtils::getMetaClassInfo(this, QLatin1String("ID"), value) ? value.toUInt() : 0;
}
