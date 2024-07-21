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

}


#endif // MMSTYPES_H
