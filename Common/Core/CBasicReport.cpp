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

const QChar eolChar(QLatin1Char('\n'));
const QChar emptyChar = QChar();
const QString vs_long(QLatin1String("VALIDATE_SCHEDULES"));
const QString vs_short(QLatin1String("V_S"));
const QString std_long(QLatin1String("SEND_TO_DAM"));
const QString std_short(QLatin1String("S_T_D"));
const QString doubleSpace(QLatin1String("  ")); //2
const QString space(QLatin1String(" "));
const QString doubleQuotes(QLatin1String("\"\""));
const QString quotes(QLatin1String("\""));

const qsizetype maxChars = 32767;

CBasicReport::CBasicReport(QObject *parent)
    : QObject{parent}
{
    m_errorString.clear();
    m_reportFileName.clear();
}

void
CBasicReport::init(pSqliteDatabase db, const QString &reportName, bool showMilliseconds)
{
    m_db = db;
    Q_CHECK_PTR(m_db);
    m_reportFileName = reportName;
    m_showMilliseconds = showMilliseconds;
}

bool
CBasicReport::generateReport(const QString &request)
{
    bool retVal = m_db->exec(request);
    if (retVal) {
        m_isMultipartReport = false;
        m_reportPartNumber = 1;
        do {
            retVal = generateReport();
        } while (m_isMultipartReport && retVal);
    } else {
        setErrorString(m_db->errorString());
    }

    return retVal;
}

void
CBasicReport::setDateTimeFormat(QXlsx::Format &dateFormat)
{
    dateFormat.setHorizontalAlignment(QXlsx::Format::AlignRight);
    QString dateTimeFormat = m_showMilliseconds ? QLatin1String("dd.mm.yyyy hh:mm:ss.000") : QLatin1String("dd.mm.yyyy hh:mm:ss");
    dateFormat.setNumberFormat(dateTimeFormat);
}

QString
CBasicReport::checkDetails(const QString &data, bool &isDataTooLong)
{
    QString buf = data;
    isDataTooLong = false;
    buf.replace(doubleQuotes, quotes);
    //ref: https://support.microsoft.com/en-us/office/excel-specifications-and-limits-1672b34d-7043-467e-8e27-269d656771c3
    if (buf.length() > maxChars) { //this line is too long
        buf.replace(eolChar, emptyChar);
        while (buf.indexOf(doubleSpace) != -1) {
            buf.replace(doubleSpace, space);
        }
        buf.replace(vs_long, vs_short);
        buf.replace(std_long, std_short);
        if (buf.length() > maxChars) {
            isDataTooLong = true;
        }
    }
    return buf;
}

void
CBasicReport::setReportDataItem(QXlsx::Document *report, const int dbFieldIndex, const int reportFieldIndex, const int row)
{
    QVariant writeValue = m_db->geValue(dbFieldIndex);
    if (!report->write(row, reportFieldIndex, writeValue))  {
        throw "Write error";
    }
}

void
CBasicReport::setReportDataItem(QXlsx::Document *report, const QString &dbFieldName, const int reportFieldIndex, const int row)
{
    QVariant writeValue = m_db->geValue(dbFieldName);
    if (!report->write(row, reportFieldIndex, writeValue)) {
        throw "Write error";
    }
}

void
CBasicReport::setReportDataItem(QXlsx::Document *report, const int column, const int row, const QVariant &writeValue)
{
    if (!report->write(row, column, writeValue)) {
        throw "Write error";
    }
}

quint16
CBasicReport::reportID() const
{
    QString value;
    return elcUtils::getMetaClassInfo(this, QLatin1String("ID"), value) ? value.toUInt() : 0;
}

QStringList
CBasicReport::sources() const
{
    QStringList retVal;
    QString value;
    if (elcUtils::getMetaClassInfo(this, QLatin1String("source"), value)) {
        retVal = value.split('|');
    }
    return retVal;
}

QString
CBasicReport::createReportPartFilename()
{
    QString fileName = m_reportFileName;
    qsizetype pos = fileName.lastIndexOf('.');
    QString buf = QString(".part%1.").arg(m_reportPartNumber, 2, 10, QChar('0'));
    fileName.replace(pos, 1, buf);
    return fileName;
}

QString
CBasicReport::createReportFilename(const int row)
{
    QString retVal = m_reportFileName;
    if (row > maxRowsCount) {
        retVal = createReportPartFilename();
        ++m_reportPartNumber;
        m_isMultipartReport = true;
        m_multipartRowCount = m_multipartRowCount + row - 2;
    } else {
        if (m_isMultipartReport) {
            retVal = createReportPartFilename();
            m_isMultipartReport = false;
        }
    }
    return retVal;
}
