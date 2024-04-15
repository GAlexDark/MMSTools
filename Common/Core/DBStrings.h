/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: DBStrings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The strings used for work with the DB.
*
****************************************************************************/

#ifndef DBSTRINGS_H
#define DBSTRINGS_H

inline const QString pragmaUTF8(QLatin1String("PRAGMA encoding = \"UTF-8\";"));
inline const QString pragmaPageSize(QLatin1String("PRAGMA page_size = %1;"));
inline const QString pragmaJournalMode(QLatin1String("PRAGMA journal_mode = %1;"));
inline const QString pragmaSynchronous(QLatin1String("PRAGMA synchronous = %1;"));
inline const QString pragmaTempStore(QLatin1String("PRAGMA temp_store = %1;"));
inline const QString pragmaLockingMode(QLatin1String("PRAGMA locking_mode = %1;"));

inline const QString phUsername(QLatin1String(":username"));
inline const QString phTimestampISO8601(QLatin1String(":timestampISO8601"));
inline const QString phTimestamp(QLatin1String(":timestamp"));
inline const QString phRequestID(QLatin1String(":requestid"));
inline const QString phType(QLatin1String(":type"));
inline const QString phDetails(QLatin1String(":details"));
inline const QString phUsername1(QLatin1String(":username1"));
inline const QString phAuthType(QLatin1String(":authtype"));
inline const QString phExternalip(QLatin1String(":externalip"));
inline const QString phInternalip(QLatin1String(":internalip"));

inline const QString phStatus(QLatin1String(":status"));
inline const QString phMethod(QLatin1String(":method"));
inline const QString phRole(QLatin1String(":role"));
inline const QString phCompanyname(QLatin1String(":companyname"));
inline const QString phAttributes(QLatin1String(":attributes"));

inline const QString person(QLatin1String("Person ["));
inline const QString nullValue(QLatin1String("null"));

//this query uses autotest
namespace eventlog {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME NOT NULL, \
PRIMARY KEY (timestampISO8601, requestid) ON CONFLICT IGNORE);"));

    inline const QString insertData(QLatin1String("INSERT OR IGNORE INTO [eventlog] (username, timestampISO8601, \
requestid, type, details, timestamp, username1, authtype, externalip, \
internalip) VALUES (:username, :timestampISO8601, :requestid, :type, \
:details, :timestamp, :username1, :authtype, :externalip, :internalip)"));

    inline const QString selectData(QLatin1String("SELECT e.timestampISO8601, e.timestamp, e.externalip, e.username, e.type, e.details, \
e.authtype, e.internalip, e.requestid FROM eventlog e %1 ORDER BY e.timestamp DESC;"));
}

namespace audittrail {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [audittraillog] (status TEXT NOT NULL, \
timestamp DATETIME NOT NULL, method TEXT NOT NULL, username TEXT NOT NULL, role TEXT NOT NULL, companyname TEXT NOT NULL, attributes TEXT, \
username1 TEXT, externalip TEXT, internalip TEXT, PRIMARY KEY (status, timestamp, method, username, role, companyname, attributes) \
ON CONFLICT IGNORE);"));

    inline const QString insertData(QLatin1String("INSERT OR IGNORE INTO [audittraillog] (status, timestamp, method, username, role, \
companyname, attributes, username1, externalip, internalip) VALUES (:status, :timestamp, :method, :username, :role, :companyname, :attributes, \
:username1, :externalip, :internalip)"));

    inline const QString selectData(QLatin1String("SELECT a.timestamp, a.username, a.role, a.companyname, a.method, a.status, \
a.attributes, a.internalip FROM audittraillog a %1 ORDER BY a.timestamp DESC;"));
}

namespace other {
    inline const QString selectEvtLogAndAuditTrailData(QLatin1String("SELECT e.timestamp as timestamp, e.username as username, null as company_role, \
e.type as type_or_method, null as status, e.details as details_or_attributes, e.authtype as authtype, e.externalip as externalip, \
e.internalip as internalip, e.requestid as requestid FROM eventlog e %1 \
UNION ALL \
SELECT a.timestamp as timestamp, a.username as username, a.companyname || ' ' || a.role as company_role, \
a.method as type_or_method, a.status as status, a.attributes as details_or_attributes, null as authtype, \
a.externalip as externalip, a.internalip as internalip, null as requestid \
FROM audittraillog a %1 ORDER BY timestamp DESC;"));
}

#endif // DBSTRINGS_H
