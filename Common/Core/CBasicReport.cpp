#include "CBasicReport.h"
#include <QMetaObject>
#include <QMetaClassInfo>

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

CBasicReport::CBasicReport()
    : m_errorString(""), m_reportFileName(""), m_db(nullptr)
{}

void
CBasicReport::init(pBasicDatabase db, const QString &reportName)
{
    m_db = db;
    Q_CHECK_PTR(m_db);
    m_reportFileName = reportName;
}

void
CBasicReport::setDateTimeFormat(QXlsx::Format &dateFormat)
{
    dateFormat.setHorizontalAlignment(QXlsx::Format::AlignRight);
    dateFormat.setNumberFormat(QLatin1String("dd.mm.yyyy hh:mm:ss"));
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

quint16
CBasicReport::reportID() const
{
    quint16 retVal = 0;
    const QMetaObject* mObj = this->metaObject();
    for (int j = mObj->classInfoOffset(); j < mObj->classInfoCount(); j++) {
        QMetaClassInfo classInfo = mObj->classInfo(j);
        if (QString(classInfo.name()) == QLatin1String("ID")) {
            retVal = QString(classInfo.value()).toUInt();
            break;
        }
    }
    return retVal;
}
