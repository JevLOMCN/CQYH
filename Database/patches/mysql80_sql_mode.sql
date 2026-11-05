-- MySQL 8.0: relax sql_mode so the game servers can create characters.
--
-- Why: dbserver's create-role statement inserts DeleteTime = 0. With the default 8.0
-- strict mode (STRICT_TRANS_TABLES + NO_ZERO_DATE / NO_ZERO_IN_DATE) MySQL rejects it
-- with error 1292 "Incorrect datetime value: '0' for column 'DeleteTime'" and the
-- client shows MMOErrorInfo.ERROR_CODE_DEF.errDatabase.
--
-- Run as root:   mysql -uroot -p < mysql80_sql_mode.sql
--
-- SET PERSIST applies the value now (for NEW connections) and writes it to
-- mysqld-auto.cnf so it survives a restart. Restart the game servers afterwards,
-- they keep the sql_mode they connected with.
SET PERSIST sql_mode = 'NO_ENGINE_SUBSTITUTION';

-- Verify
SELECT VERSION() AS version, @@GLOBAL.sql_mode AS global_sql_mode;

-- Note: the game servers still link a MySQL 5.1 client (libmysql64_d.dll), which only speaks
-- mysql_native_password. On 8.0 this plugin is on by default, so root created with the
-- default plugin works. If a user was created with caching_sha2_password, switch it:
--   ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '1111';
-- (On 8.4+ also set mysql_native_password=ON in my.ini; it is off by default there.)
