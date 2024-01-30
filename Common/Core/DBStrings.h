#ifndef DBSTRINGS_H
#define DBSTRINGS_H

inline const QString getAllRecords = QStringLiteral("SELECT e.timestampISO8601, e.timestamp, e.externalip, e.username, e.type, e.details, \
e.authtype, e.internalip, e.requestid FROM eventlog e %1 ORDER BY e.timestamp DESC;");

inline const QString pragmaUTF8 = QStringLiteral("PRAGMA encoding = \"UTF-8\";");
inline const QString pragmaPageSize = QStringLiteral("PRAGMA page_size = %1;");

inline const QString insertOriginalData = QStringLiteral("INSERT OR IGNORE INTO [eventlog] (username, timestampISO8601, \
requestid, type, details, timestamp, username1, authtype, externalip, \
internalip) VALUES (:username, :timestampISO8601, :requestid, :type, \
:details, :timestamp, :username1, :authtype, :externalip, :internalip)");

inline const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME NOT NULL, \
PRIMARY KEY (timestampISO8601, requestid) ON CONFLICT IGNORE);");

inline const QString pragmaJournalMode = QStringLiteral("PRAGMA journal_mode = %1;");
inline const QString pragmaSynchronous = QStringLiteral("PRAGMA synchronous = %1;");
inline const QString pragmaTempStore = QStringLiteral("PRAGMA temp_store = %1;");
inline const QString pragmaLockingMode = QStringLiteral("PRAGMA locking_mode = %1;");

#endif // DBSTRINGS_H
