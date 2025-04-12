#include "CAuditTrailParserBase.h"
#include <QRegularExpression>
#include <QString>

namespace {
    const QRegularExpression rePersonData(QLatin1String("Person\\s\\[pk=\\d+,\\salias=(.*)\\]"));
    const QRegularExpression reLoadAuditTrail(QLatin1String("^.*;.*;(.*);.*;.*;.*;.*;.*;.*$"));

    const QString person(QLatin1String("Person ["));
    const QString nullValue(QLatin1String("null"));
}

bool
CAuditTrailParserBase::parsePersonDataDetails()
{
    qsizetype firstPos = m_attributes.indexOf(person);
    if (firstPos == -1) {
        return false;
    }
    qsizetype lastPos = m_attributes.indexOf(QLatin1Char(']'), firstPos);
    QString alias = m_attributes.sliced(firstPos, lastPos - firstPos + 1).trimmed();
    QRegularExpressionMatch match = rePersonData.match(alias);
    if (!match.hasMatch()) {
        return false;
    }
    m_username1 = match.captured(1);
    return true;
}

bool
CAuditTrailParserBase::parseLoadAuditTrail()
{
    QRegularExpressionMatch match = reLoadAuditTrail.match(m_attributes);
    if (!match.hasMatch()) {
        return false;
    }
    m_username1 = match.captured(1);
    if (m_username1.compare(nullValue, Qt::CaseInsensitive) == 0) {
        m_username1.clear();
    }
    return true;
}

bool
CAuditTrailParserBase::parseAttributesDetails()
{
    return parsePersonDataDetails();
}
