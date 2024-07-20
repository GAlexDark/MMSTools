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

#include <QException>

namespace mms {
    struct fileFieldsSeparation_t
    {
        char delimiterChar; // , ; \t space
        char quoteChar; // ' "
        QByteArray eolChars; // \n \r\n
    };
    typedef fileFieldsSeparation_t ffs_t;

    typedef QMap<QString, QString> pragmaList_t;
    typedef QMap<QString, QVariant> dataItem_t;

    //reserved to the future
    enum class MmsLogsTypes { LT_NONE, LT_EVENTLOG_MMS, LT_EVENTLOG_REPORT, LT_AUDITTRAIL_MMS, LT_AUDITTRAIL_REPORT };
    enum class MmsReportsTypes {RT_NONE, RT_EVENTLOG, RT_AUDITTRAIL, RT_COMBINED };

    class MmsCommonException : public QException
    {
    private:
        char *m_message;

    public:
        explicit MmsCommonException(char *text = nullptr) noexcept
            :m_message(text) {}
        virtual ~MmsCommonException() {}
        void raise() const override { throw *this; }
        MmsCommonException *clone() const override { return new MmsCommonException(*this); }
        const char *what() const noexcept override
        {
            return m_message;
        }
    };

    class XlsxError : public MmsCommonException
    {
    public:
        explicit XlsxError(char *text = "QXlsx write error") noexcept
            :MmsCommonException(text) {}
    };
}


#endif // MMSTYPES_H
