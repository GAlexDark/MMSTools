/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: MMSTypes.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The MMS types
*
****************************************************************************/

#ifndef MMSTYPES_H
#define MMSTYPES_H

namespace mms {
    struct fileFieldsSeparation_t
    {
        char delimiterChar; // , ; \t space
        char quoteChar; // ' "
        QByteArray eolChars; // \n \r\n
    };
    using ffs_t = fileFieldsSeparation_t;
    using pragmaList_t = QMap<QString, QString>;
    using dataItem_t = QMap<QString, QVariant>;

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
