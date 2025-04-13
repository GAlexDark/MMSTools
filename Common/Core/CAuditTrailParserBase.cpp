/****************************************************************************
*
* Class for parsing the MMS Audit Trail logs.
* Copyright (C) 2025  Oleksii Gaienko
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
