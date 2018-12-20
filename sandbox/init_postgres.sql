
/* run this script with psql -f $FILE */
\set current_user `echo $USER`
CREATE DATABASE fs_db;
CREATE USER :current_user;
GRANt ALL ON DATABASE fs_db TO :current_user;