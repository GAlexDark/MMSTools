#ifndef MMSTYPES_H
#define MMSTYPES_H

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
    enum MmsLogsTypes { LT_NONE, LT_EVENTLOG_MMS, LT_EVENTLOG_REPORT, LT_AUDITTRAIL_MMS, LT_AUDITTRAIL_REPORT };
    enum MmsReportsTypes {RT_NONE, RT_EVENTLOG, RT_AUDITTRAIL, RT_COMBINED };

}


#endif // MMSTYPES_H
