//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 Underscore AB
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 *      Specifically you are not permitted to link this program with the
 *      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
 *      And you are not permitted to distribute binaries compiled against
 *      these libraries without written consent from Underscore AB. Observe
 *      that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

static char *DefaultKeywords[]={
  "ABORT",
  "ACCESS",
  "ACCESSED",
  "ACCOUNT",
  "ACTIVATE",
  "ADMIN",
  "ADMINISTER",
  "ADMINISTRATOR",
  "ADVISE",
  "AFTER",
  "ALGORITHM",
  "ALL",
  "ALLOCATE",
  "ALLOW",
  "ALL_ROWS",
  "ALTER",
  "ALWAYS",
  "ANALYZE",
  "ANCILLARY",
  "AND",
  "ANY",
  "APPLY",
  "ARCHIVE",
  "ARCHIVELOG",
  "ARRAY",
  "AS",
  "ASC",
  "ASSOCIATE",
  "AT",
  "ATTRIBUTE",
  "ATTRIBUTES",
  "AUDIT",
  "AUTHENTICATED",
  "AUTHID",
  "AUTHORIZATION",
  "AUTO",
  "AUTOALLOCATE",
  "AUTOEXTEND",
  "AUTOMATIC",
  "BACKUP",
  "BECOME",
  "BEFORE",
  "BEGIN",
  "BEHALF",
  "BETWEEN",
  "BFILE",
  "BINDING",
  "BITMAP",
  "BITS",
  "BLOB",
  "BLOCK",
  "BLOCKSIZE",
  "BLOCK_RANGE",
  "BODY",
  "BOTH",
  "BOUND",
  "BROADCAST",
  "BUFFER_POOL",
  "BUILD",
  "BULK",
  "BY",
  "BYTE",
  "CACHE",
  "CACHE_INSTANCES",
  "CALL",
  "CANCEL",
  "CASCADE",
  "CASE",
  "CAST",
  "CATEGORY",
  "CERTIFICATE",
  "CFILE",
  "CHAINED",
  "CHANGE",
  "CHAR",
  "CHARACTER",
  "CHAR_CS",
  "CHECK",
  "CHECKPOINT",
  "CHILD",
  "CHOOSE",
  "CHUNK",
  "CLEAR",
  "CLOB",
  "CLONE",
  "CLOSE",
  "CLOSE_CACHED_OPEN_CURSORS",
  "CLUSTER",
  "COALESCE",
  "COLLECT",
  "COLUMN",
  "COLUMNS",
  "COLUMN_VALUE",
  "COMMENT",
  "COMMIT",
  "COMMITTED",
  "COMPATIBILITY",
  "COMPILE",
  "COMPLETE",
  "COMPOSITE_LIMIT",
  "COMPRESS",
  "COMPUTE",
  "CONFORMING",
  "CONNECT",
  "CONNECT_TIME",
  "CONSIDER",
  "CONSISTENT",
  "CONSTRAINT",
  "CONSTRAINTS",
  "CONTAINER",
  "CONTENTS",
  "CONTEXT",
  "CONTINUE",
  "CONTROLFILE",
  "CONVERT",
  "CORRUPTION",
  "COST",
  "CPU_PER_CALL",
  "CPU_PER_SESSION",
  "CREATE",
  "CREATE_STORED_OUTLINES",
  "CROSS",
  "CUBE",
  "CURRENT",
  "CURRENT_DATE",
  "CURRENT_SCHEMA",
  "CURRENT_TIME",
  "CURRENT_TIMESTAMP",
  "CURRENT_USER",
  "CURSOR",
  "CURSOR_SPECIFIC_SEGMENT",
  "CYCLE",
  "DANGLING",
  "DATA",
  "DATABASE",
  "DATAFILE",
  "DATAFILES",
  "DATAOBJNO",
  "DATE",
  "DATE_MODE",
  "DBA",
  "DBTIMEZONE",
  "DDL",
  "DEALLOCATE",
  "DEBUG",
  "DEC",
  "DECIMAL",
  "DECLARE",
  "DECODE",
  "DEFAULT",
  "DEFERRABLE",
  "DEFERRED",
  "DEFINED",
  "DEFINER",
  "DEGREE",
  "DELAY",
  "DELETE",
  "DEMAND",
  "DENSE_RANK",
  "DEREF",
  "DESC",
  "DETACHED",
  "DETERMINES",
  "DICTIONARY",
  "DIMENSION",
  "DIRECTORY",
  "DISABLE",
  "DISASSOCIATE",
  "DISCONNECT",
  "DISMOUNT",
  "DISPATCHERS",
  "DISTINCT",
  "DISTINGUISHED",
  "DISTRIBUTED",
  "DML",
  "DOUBLE",
  "DROP",
  "DUMP",
  "DYNAMIC",
  "EACH",
  "ELEMENT",
  "ELSE",
  "ELSIF",
  "ENABLE",
  "ENCRYPTED",
  "ENCRYPTION",
  "END",
  "ENFORCE",
  "ENTRY",
  "ERROR_ON_OVERLAP_TIME",
  "ESCAPE",
  "ESTIMATE",
  "EVENTS",
  "EXCEPT",
  "EXCEPTION",
  "EXCEPTIONS",
  "EXCHANGE",
  "EXCLUDING",
  "EXCLUSIVE",
  "EXECUTE",
  "EXEMPT",
  "EXISTS",
  "EXIT",
  "EXPIRE",
  "EXPLAIN",
  "EXPLOSION",
  "EXTEND",
  "EXTENDS",
  "EXTENT",
  "EXTENTS",
  "EXTERNAL",
  "EXTERNALLY",
  "EXTRACT",
  "FAILED_LOGIN_ATTEMPTS",
  "FALSE",
  "FAST",
  "FILE",
  "FILTER",
  "FINAL",
  "FINISH",
  "FIRST",
  "FIRST_ROWS",
  "FLAGGER",
  "FLOAT",
  "FLOB",
  "FLUSH",
  "FOLLOWING",
  "FOR",
  "FORCE",
  "FOREIGN",
  "FREELIST",
  "FREELISTS",
  "FRESH",
  "FROM",
  "FULL",
  "FUNCTION",
  "FUNCTIONS",
  "GENERATED",
  "GLOBAL",
  "GLOBALLY",
  "GLOBAL_NAME",
  "GLOBAL_TOPIC_ENABLED",
  "GRANT",
  "GROUP",
  "GROUPS",
  "GUARD",
  "HASH",
  "HASHKEYS",
  "HAVING",
  "HEADER",
  "HEAP",
  "HIERARCHY",
  "HOUR",
  "IDENTIFIED",
  "IDENTIFIER",
  "IDGENERATORS",
  "IDLE_TIME",
  "IF",
  "IMMEDIATE",
  "IN",
  "INCLUDING",
  "INCREMENT",
  "INCREMENTAL",
  "INDEX",
  "INDEXED",
  "INDEXES",
  "INDEXTYPE",
  "INDEXTYPES",
  "INDICATOR",
  "INITIAL",
  "INITIALIZED",
  "INITIALLY",
  "INITRANS",
  "INNER",
  "INSERT",
  "INSTANCE",
  "INSTANCES",
  "INSTANTIABLE",
  "INSTEAD",
  "INT",
  "INTEGER",
  "INTEGRITY",
  "INTERMEDIATE",
  "INTERNAL_CONVERT",
  "INTERNAL_USE",
  "INTERSECT",
  "INTERVAL",
  "INTO",
  "INVALIDATE",
  "IN_MEMORY_METADATA",
  "IS",
  "ISOLATION",
  "ISOLATION_LEVEL",
  "JAVA",
  "JOIN",
  "KEEP",
  "KERBEROS",
  "KEY",
  "KEYFILE",
  "KEYS",
  "KEYSIZE",
  "KILL",
  "LABEL",
  "LAST",
  "LATERAL",
  "LAYER",
  "LDAP_REGISTRATION",
  "LDAP_REGISTRATION_ENABLED",
  "LDAP_REG_SYNC_INTERVAL",
  "LEADING",
  "LEFT",
  "LESS",
  "LEVEL",
  "LEVELS",
  "LIBRARY",
  "LIKE",
  "LIKE2",
  "LIKE4",
  "LIKEC",
  "LIMIT",
  "LINK",
  "LIST",
  "LOB",
  "LOCAL",
  "LOCALTIME",
  "LOCALTIMESTAMP",
  "LOCATION",
  "LOCATOR",
  "LOCK",
  "LOCKED",
  "LOG",
  "LOGFILE",
  "LOGGING",
  "LOGICAL",
  "LOGICAL_READS_PER_CALL",
  "LOGICAL_READS_PER_SESSION",
  "LOGOFF",
  "LOGON",
  "LONG",
  "LOOP",
  "MANAGE",
  "MANAGED",
  "MANAGEMENT",
  "MANUAL",
  "MAPPING",
  "MASTER",
  "MATCHED",
  "MATERIALIZED",
  "MAXARCHLOGS",
  "MAXDATAFILES",
  "MAXEXTENTS",
  "MAXINSTANCES",
  "MAXLOGFILES",
  "MAXLOGHISTORY",
  "MAXLOGMEMBERS",
  "MAXSIZE",
  "MAXTRANS",
  "MAXVALUE",
  "MEMBER",
  "MEMORY",
  "MERGE",
  "METHOD",
  "MINEXTENTS",
  "MINIMIZE",
  "MINIMUM",
  "MINUS",
  "MINVALUE",
  "MLSLABEL",
  "MODE",
  "MODIFY",
  "MONITORING",
  "MOUNT",
  "MOVE",
  "MOVEMENT",
  "MTS_DISPATCHERS",
  "MULTISET",
  "NAMED",
  "NCHAR",
  "NCHAR_CS",
  "NCLOB",
  "NEEDED",
  "NESTED",
  "NESTED_TABLE_ID",
  "NETWORK",
  "NEVER",
  "NEXT",
  "NLS_CALENDAR",
  "NLS_CHARACTERSET",
  "NLS_COMP",
  "NLS_CURRENCY",
  "NLS_DATE_FORMAT",
  "NLS_DATE_LANGUAGE",
  "NLS_ISO_CURRENCY",
  "NLS_LANG",
  "NLS_LANGUAGE",
  "NLS_LENGTH_SEMANTICS",
  "NLS_NCHAR_CONV_EXCP",
  "NLS_NUMERIC_CHARACTERS",
  "NLS_SORT",
  "NLS_SPECIAL_CHARS",
  "NLS_TERRITORY",
  "NO",
  "NOARCHIVELOG",
  "NOAUDIT",
  "NOCACHE",
  "NOCOMPRESS",
  "NOCYCLE",
  "NODELAY",
  "NOFORCE",
  "NOLOGGING",
  "NOMAPPING",
  "NOMAXVALUE",
  "NOMINIMIZE",
  "NOMINVALUE",
  "NOMONITORING",
  "NONE",
  "NOORDER",
  "NOOVERRIDE",
  "NOPARALLEL",
  "NORELY",
  "NORESETLOGS",
  "NOREVERSE",
  "NORMAL",
  "NOROWDEPENDENCIES",
  "NOSEGMENT",
  "NOSORT",
  "NOSWITCH",
  "NOT",
  "NOTHING",
  "NOVALIDATE",
  "NOWAIT",
  "NULL",
  "NULLS",
  "NUMBER",
  "NUMERIC",
  "NVARCHAR2",
  "OBJECT",
  "OBJNO",
  "OBJNO_REUSE",
  "OF",
  "OFF",
  "OFFLINE",
  "OID",
  "OIDINDEX",
  "OLD",
  "ON",
  "ONLINE",
  "ONLY",
  "OPAQUE",
  "OPCODE",
  "OPEN",
  "OPERATOR",
  "OPTIMAL",
  "OPTIMIZER_GOAL",
  "OPTION",
  "OR",
  "ORDER",
  "ORGANIZATION",
  "OTHERS",
  "OUTER",
  "OUTLINE",
  "OVER",
  "OVERFLOW",
  "OVERLAPS",
  "OWN",
  "PACKAGE",
  "PACKAGES",
  "PARALLEL",
  "PARAMETERS",
  "PARENT",
  "PARTITION",
  "PARTITIONS",
  "PARTITION_HASH",
  "PARTITION_LIST",
  "PARTITION_RANGE",
  "PASSWORD",
  "PASSWORD_GRACE_TIME",
  "PASSWORD_LIFE_TIME",
  "PASSWORD_LOCK_TIME",
  "PASSWORD_REUSE_MAX",
  "PASSWORD_REUSE_TIME",
  "PASSWORD_VERIFY_FUNCTION",
  "PCTFREE",
  "PCTINCREASE",
  "PCTTHRESHOLD",
  "PCTUSED",
  "PCTVERSION",
  "PERCENT",
  "PERMANENT",
  "PFILE",
  "PHYSICAL",
  "PLAN",
  "PLSQL_DEBUG",
  "POLICY",
  "POST_TRANSACTION",
  "PREBUILT",
  "PRECEDING",
  "PRECISION",
  "PREPARE",
  "PRESERVE",
  "PRIMARY",
  "PRIOR",
  "PRIVATE",
  "PRIVATE_SGA",
  "PRIVILEGE",
  "PRIVILEGES",
  "PROCEDURE",
  "PROFILE",
  "PROTECTED",
  "PUBLIC",
  "PURGE",
  "PX_GRANULE",
  "QUERY",
  "QUEUE",
  "QUIESCE",
  "QUOTA",
  "RANDOM",
  "RANGE",
  "RAW",
  "RBA",
  "READ",
  "READS",
  "REAL",
  "REBUILD",
  "RECORDS_PER_BLOCK",
  "RECOVER",
  "RECOVERABLE",
  "RECOVERY",
  "RECYCLE",
  "REDUCED",
  "REF",
  "REFERENCES",
  "REFERENCING",
  "REFRESH",
  "REGISTER",
  "REJECT",
  "REKEY",
  "RELATIONAL",
  "RELY",
  "RENAME",
  "REPLACE",
  "RESET",
  "RESETLOGS",
  "RESIZE",
  "RESOLVE",
  "RESOLVER",
  "RESOURCE",
  "RESTRICT",
  "RESTRICTED",
  "RESUMABLE",
  "RESUME",
  "RETURN",
  "RETURNING",
  "REUSE",
  "REVERSE",
  "REVOKE",
  "REWRITE",
  "RIGHT",
  "ROLE",
  "ROLES",
  "ROLLBACK",
  "ROLLUP",
  "ROWDEPENDENCIES",
  "ROWID",
  "ROWNUM",
  "ROWS",
  "RULE",
  "SAMPLE",
  "SAVEPOINT",
  "SB4",
  "SCAN",
  "SCAN_INSTANCES",
  "SCHEMA",
  "SCN",
  "SCOPE",
  "SD_ALL",
  "SD_INHIBIT",
  "SD_SHOW",
  "SECURITY",
  "SEGMENT",
  "SEG_BLOCK",
  "SEG_FILE",
  "SELECT",
  "SELECTIVITY",
  "SEQUENCE",
  "SEQUENCED",
  "SERIALIZABLE",
  "SERVERERROR",
  "SESSION",
  "SESSIONS_PER_USER",
  "SESSIONTIMEZONE",
  "SESSIONTZNAME",
  "SESSION_CACHED_CURSORS",
  "SET",
  "SETS",
  "SHARE",
  "SHARED",
  "SHARED_POOL",
  "SHRINK",
  "SHUTDOWN",
  "SIBLINGS",
  "SINGLE",
  "SINGLETASK",
  "SIZE",
  "SKIP",
  "SKIP_UNUSABLE_INDEXES",
  "SMALLINT",
  "SNAPSHOT",
  "SOME",
  "SORT",
  "SOURCE",
  "SPACE",
  "SPECIFICATION",
  "SPFILE",
  "SPLIT",
  "SQL_TRACE",
  "STANDBY",
  "START",
  "STARTUP",
  "STATEMENT_ID",
  "STATIC",
  "STATISTICS",
  "STOP",
  "STORAGE",
  "STORE",
  "STRUCTURE",
  "SUBPARTITION",
  "SUBPARTITIONS",
  "SUBPARTITION_REL",
  "SUBSTITUTABLE",
  "SUCCESSFUL",
  "SUMMARY",
  "SUPPLEMENTAL",
  "SUSPEND",
  "SWITCH",
  "SWITCHOVER",
  "SYNONYM",
  "SYSDBA",
  "SYSOPER",
  "SYSTEM",
  "SYSTIMESTAMP",
  "SYS_OP_BITVEC",
  "SYS_OP_COL_PRESENT",
  "SYS_OP_ENFORCE_NOT_NULL$",
  "SYS_OP_MINE_VALUE",
  "SYS_OP_NOEXPAND",
  "SYS_OP_NTCIMG$",
  "TABLE",
  "TABLES",
  "TABLESPACE",
  "TABLESPACE_NO",
  "TABNO",
  "TEMPFILE",
  "TEMPORARY",
  "THAN",
  "THEN",
  "THREAD",
  "THROUGH",
  "TIMEOUT",
  "TIMEZONE_ABBR",
  "TIMEZONE_HOUR",
  "TIMEZONE_MINUTE",
  "TIMEZONE_REGION",
  "TIME_ZONE",
  "TO",
  "TOPLEVEL",
  "TRACE",
  "TRACING",
  "TRAILING",
  "TRANSACTION",
  "TRANSITIONAL",
  "TREAT",
  "TRIGGER",
  "TRIGGERS",
  "TRUE",
  "TRUNCATE",
  "TX",
  "TYPES",
  "TZ_OFFSET",
  "UB2",
  "UBA",
  "UID",
  "UNARCHIVED",
  "UNBOUND",
  "UNBOUNDED",
  "UNDER",
  "UNDO",
  "UNIFORM",
  "UNION",
  "UNIQUE",
  "UNLIMITED",
  "UNLOCK",
  "UNPROTECTED",
  "UNQUIESCE",
  "UNRECOVERABLE",
  "UNTIL",
  "UNUSABLE",
  "UNUSED",
  "UPDATABLE",
  "UPDATE",
  "UPD_INDEXES",
  "UPD_JOININDEX",
  "UPGRADE",
  "UROWID",
  "USAGE",
  "USE",
  "USER_DEFINED",
  "USE_PRIVATE_OUTLINES",
  "USE_STORED_OUTLINES",
  "USING",
  "VALIDATE",
  "VALIDATION",
  "VALUES",
  "VARCHAR",
  "VARCHAR2",
  "VARRAY",
  "VARYING",
  "VIEW",
  "WAIT",
  "WHEN",
  "WHENEVER",
  "WHERE",
  "WITH",
  "WITHIN",
  "WITHOUT",
  "WORK",
  "WRITE",
  "XID",
  "XMLSCHEMA",
  "XMLTYPE",
  "ZONE",
  NULL
};
