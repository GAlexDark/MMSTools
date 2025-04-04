/****************************************************************************
*
* The MMS types.
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

#ifndef MMSTYPES_H
#define MMSTYPES_H

#include <QByteArray>
#include <QScopedPointer>
#include <QString>

namespace mms {
    struct fileFieldsSeparation_t
    {
        char delimiterChar; // , ; \t space
        char quoteChar; // ' "
        QByteArray eolChars; // \n \r\n
    };
    using ffs_t = fileFieldsSeparation_t;

    //reserved to the future
    enum class MmsLogsTypes { LT_NONE, LT_EVENTLOG_MMS, LT_EVENTLOG_REPORT, LT_AUDITTRAIL_MMS, LT_AUDITTRAIL_REPORT };
    enum class MmsReportsTypes {RT_NONE, RT_EVENTLOG, RT_AUDITTRAIL, RT_COMBINED };

    class MmsCommonException : public std::exception
    {
    private:
        QScopedPointer<char> m_message;

    public:
        explicit MmsCommonException(const QString &text) noexcept
        {
            try {
                size_t maxBufSize = 4096;
                m_message.reset(new char [maxBufSize]);
                std::string buf = text.toStdString();
                const char *source = buf.c_str();
                errno_t retVal = strncpy_s(m_message.data(), maxBufSize, source, buf.length());
                if (retVal != 0) {
                    assert(false);
                }
            } catch (...) {
                assert(false);
            }
        }
        const char *what() const noexcept override
        {
            return m_message.data();
        }
    };
}

#endif // MMSTYPES_H
