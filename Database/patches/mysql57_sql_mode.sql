-- MySQL 5.7: relax sql_mode so the game servers can create characters.
--
-- Why: dbserver's create-role statement inserts DeleteTime = 0. With the default 5.7
-- strict mode (STRICT_TRANS_TABLES + NO_ZERO_DATE / NO_ZERO_IN_DATE) MySQL rejects it
-- with error 1292 "Incorrect datetime value: '0' for column 'DeleteTime'" and the
-- client shows MMOErrorInfo.ERROR_CODE_DEF.errDatabase.
--
-- Run as root:   mysql -uroot -p < mysql57_sql_mode.sql
--
-- 1) Takes effect immediately for NEW connections (restart the game servers afterwards,
--    they keep the sql_mode they connected with).
SET GLOBAL sql_mode = 'NO_ENGINE_SUBSTITUTION';

-- 2) MySQL 5.7 has no SET PERSIST, so the change is lost when MySQL restarts unless it is
--    also written to my.ini (Windows) / my.cnf (Linux) under [mysqld]:
--
--        [mysqld]
--        sql_mode=NO_ENGINE_SUBSTITUTION
--
--    On this setup the server runs from c:\mysql\ (basedir), so the file is c:\mysql\my.ini.

-- Verify
SELECT VERSION() AS version, @@GLOBAL.sql_mode AS global_sql_mode;
