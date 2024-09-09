/****************************************************************************
*
* The strings used for work with the Database.
* Copyright (C) 2023-2024  Oleksii Gaienko
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

#ifndef DBSTRINGS_H
#define DBSTRINGS_H

#include <QString>

inline const QString pragmaUTF8(QLatin1String("PRAGMA encoding = \"UTF-8\";"));
inline const QString pragmaPageSize(QLatin1String("PRAGMA page_size = %1;"));
inline const QString pragmaJournalMode(QLatin1String("PRAGMA journal_mode = %1;"));
inline const QString pragmaSynchronous(QLatin1String("PRAGMA synchronous = %1;"));
inline const QString pragmaTempStore(QLatin1String("PRAGMA temp_store = %1;"));
inline const QString pragmaLockingMode(QLatin1String("PRAGMA locking_mode = %1;"));
inline const QString pragmaMmapSize(QLatin1String("PRAGMA mmap_size = 2147418112;"));
inline const QString pragmaThreads(QLatin1String("PRAGMA threads = 8;"));

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

inline const QString phSeverity(QLatin1String(":severity"));
inline const QString phMessage(QLatin1String(":message"));

inline const QString phNumber(QLatin1String(":number"));
inline const QString phParticipantEic(QLatin1String(":participantEic"));
inline const QString phDateToWhichRelated(QLatin1String(":dateToWhichRelated"));

//this query uses autotest
namespace eventlog {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME NOT NULL, PRIMARY KEY (timestampISO8601, requestid) ON CONFLICT IGNORE);"));
    inline const QString insertData(QLatin1String("INSERT OR IGNORE INTO [eventlog] (username, timestampISO8601, requestid, type, details, timestamp, username1, authtype, externalip, internalip) VALUES (:username, :timestampISO8601, :requestid, :type, :details, :timestamp, :username1, :authtype, :externalip, :internalip)"));
    inline const QString selectData(QLatin1String("SELECT e.timestampISO8601, e.timestamp, e.externalip, e.username, e.type, e.details, e.authtype, e.internalip, e.requestid FROM eventlog e %1 ORDER BY e.timestamp DESC;"));
}

namespace audittrail {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [audittraillog] (status TEXT NOT NULL, timestamp DATETIME NOT NULL, method TEXT NOT NULL, username TEXT NOT NULL, role TEXT NOT NULL, companyname TEXT NOT NULL, attributes TEXT, username1 TEXT, externalip TEXT, internalip TEXT, PRIMARY KEY (status, timestamp, method, username, role, companyname, attributes) ON CONFLICT IGNORE);"));
    inline const QString insertData(QLatin1String("INSERT OR IGNORE INTO [audittraillog] (status, timestamp, method, username, role, companyname, attributes, username1, externalip, internalip) VALUES (:status, :timestamp, :method, :username, :role, :companyname, :attributes, :username1, :externalip, :internalip)"));
    inline const QString selectData(QLatin1String("SELECT a.timestamp, a.username, a.role, a.companyname, a.method, a.status, a.attributes, a.internalip FROM audittraillog a %1 ORDER BY a.timestamp DESC;"));
}

namespace other {
    inline const QString selectEvtLogAndAuditTrailData(QLatin1String("SELECT e.timestamp as timestamp, e.username as username, null as company_role, e.type as type_or_method, null as status, e.details as details_or_attributes, e.authtype as authtype, e.externalip as externalip, e.internalip as internalip, e.requestid as requestid FROM eventlog e %1 UNION ALL SELECT a.timestamp as timestamp, a.username as username, a.companyname || ' ' || a.role as company_role, a.method as type_or_method, a.status as status, a.attributes as details_or_attributes, null as authtype, a.externalip as externalip, a.internalip as internalip, null as requestid FROM audittraillog a %1 UNION ALL SELECT s.timestamp as timestamp, s.username1 as username, s.companyname || ' ' || s.role as company_role, s.type as type_or_method, s.severity as status, s.message as details_or_attributes, null as authtype, null as externalip, null as internalip, null as requestid FROM systemlog s %1 ORDER BY timestamp ASC;"));
}

namespace systemlog {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [systemlog] (severity TEXT NOT NULL, timestamp DATETIME NOT NULL, message TEXT NOT NULL, username TEXT, username1 TEXT, role TEXT, companyname TEXT, type TEXT, PRIMARY KEY (severity, timestamp, message) ON CONFLICT IGNORE);"));
    inline const QString insertData(QLatin1String("INSERT OR IGNORE INTO [systemlog] (severity, timestamp, message, username, username1, role, companyname, type)  VALUES (:severity, :timestamp, :message, :username, :username1, :role, :companyname, :type)"));
    inline const QString selectData(QLatin1String("SELECT a.timestamp, a.username1, a.role, a.companyname, a.type, a.severity, a.message FROM systemlog a %1 ORDER BY a.timestamp DESC;"));
}

namespace MotActivations {
    inline const QString createTable(QLatin1String("CREATE TABLE IF NOT EXISTS [mot_activations_request_data] (number INTEGER NOT NULL, participant_eic TEXT NOT NULL, date_to_which_related DATE NOT NULL);"));
    inline const QString insertData(QLatin1String("INSERT INTO [mot_activations_request_data] (number, participant_eic, date_to_which_related) VALUES (:number, :participant_eic, :date_to_which_related)"));
    inline const QString selectRequestData(QLatin1String("SELECT DISTINCT participant_eic, date_to_which_related FROM mot_activations_request_data;"));
    inline const QString createResponseDataTable(QLatin1String("CREATE TABLE IF NOT EXISTS [mot_activations_response_data] (day TEXT NOT NULL, hour TEXT NOT NULL, call_start TEXT NOT NULL, call_end TEXT NOT NULL, offer_volume TEXT NOT NULL, mba TEXT NOT NULL, oom TEXT NOT NULL, direction TEXT NOT NULL, price TEXT NOT NULL, company TEXT NOT NULL, company_alias TEXT NOT NULL, resource TEXT NOT NULL, resource_alias TEXT NOT NULL, bidid TEXT NOT NULL);"));
    inline const QString insertResponseData(QLatin1String("INSERT INTO [mot_activations_response_data] (day, hour, call_start, call_end, offer_volume, mba, oom, direction, price, company, company_alias, resource, resource_alias, bidid) VALUES (:day, :hour, :call_start, :call_end, :offer_volume, :mba, :oom, :direction, :price, :company, :company_alias, :resource, :resource_alias, :bidid)"));
    inline const QString selectResponseData(QLatin1String("SELECT convert(date, us.LOCAL) as [DAY], concat (datepart(hour, us.local), ' - ', datepart(hour, dateadd(hour, 1, us.LOCAL))) as [Hour], convert(time, (left(US.LOCAL, 14) + right(c.startDate, 9))) AS [Call Start], convert(time, (left(Ue.LOCAL, 14) + right(c.endDate, 9))) AS [Call End], o.volume as [Offer Volume], O.marketBalanceAreaEic AS [MBA], c.outOfMerit AS [OOM], CASE WHEN m.direction = 'D' THEN 'UP' WHEN m.direction ='P' THEN 'DOWN' END as [Direction], o.price AS [Price], mp.eic AS [Company], mp.alias AS [Company alias], s.eic AS [Resource], cm.alias AS [Resource alias], O.id AS [BIDid] FROM emdbukrenergomms_mot.dbo.motCall C left join emdbukrenergomms_mot.dbo.motMolEntry E on E.pk = C.molEntryPk left join emdbukrenergomms_mot.dbo.motMol M on M.pk = E.molPk left join emdbukrenergomms_mot.dbo.motOffer O on O.pk = E.offerPk left join emdbukrenergomms_mot.dbo.motSupplier s on s.pk = o.supplierPk left join emdbukrenergomms_mot.dbo.emtCompany Cm on Cm.eic = s.eic left join emdbukrenergomms.dbo.emtCompany mp on mp.eic like left(cm.comRegNumber, 16) left join ReportServer.dbo.UTC_LOCAL us on left(us.UTC, 14) = left(c.startDate, 14) left join ReportServer.dbo.UTC_LOCAL ue on left(ue.UTC, 14) = left(c.endDate, 14) WHERE convert(date, us.LOCAL) between :from and :to ORDER BY o.startDate;"));
}

#endif // DBSTRINGS_H
