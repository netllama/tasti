# **Tasti** - a social bookmarking web application #

## Introduction ##

**Tasti** is a social bookmarking web application written PHP, which uses a
PostgreSQL database backend.  It was conceived following the aftermath of
Yahoo's "Delicious Sunsetting" fiasco, when I decided that I could no longer
trust any third party to store my bookmarks.  I'm developing **Tasti** in my
(limited) free time, however, I intend to use it as my primary bookmarking
tool.  Tasti supports multiple users, who can share their bookmarks with each
other.

## Requirements ##

  * An Apache web server (tested with httpd-2.2.17).  It might very well work with other web servers, however that is untested.
  * PHP support (including the PostgreSQL database module).  Tested with php-5.3.5, however any php-5.x release should work
  * A functional PostgreSQL database installation.  Tested with PostgreSQL-8.4.6, however any 8.x or 9.x release should work.  **Tasti** is completely untested with any other RDMS (this includes MySQL).  If it works with any other SQL database, that would be a complete accident.

## Installation ##

  1. Untar the **Tasti** tarball somewhere under your Apache document root.  The following example assumes that you'll be placing tasti in /var/www/html/tasti. Thus, if your domain name is 'example.com', you would access tasti in your web browser by going to http://example.com/tasti :
```
	# cd /var/www/html
	# tar xzvf tasti-1.0.tgz
```
  1. Create a new database for **Tasti**, and optionally a new database user which will be used to connect to the new database.  The following example commands assume that you will create a new database user named 'tasti' and a new database, also named 'tasti':
```
	$ createuser tasti  # this step is optional if you already have a separate user available
	$ createdb tasti
```
  1. Setup the database schema by running the included tasti.sql file with psql.  The following example continues the example from the previous step, by using the 'tasti' database user and 'tasti' database name:
```
	$ psql -U tasti -d tasti -f tasti.sql
	psql:tasti.sql:1: NOTICE:  CREATE TABLE will create implicit sequence "users_id_seq" for serial column "users.id"
	psql:tasti.sql:1: NOTICE:  CREATE TABLE / UNIQUE will create implicit index "users_username_key" for table "users"
	CREATE TABLE
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	psql:tasti.sql:9: NOTICE:  CREATE TABLE will create implicit sequence "bmarks_id_seq" for serial column "bmarks.id"
	CREATE TABLE
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	psql:tasti.sql:20: NOTICE:  CREATE TABLE will create implicit sequence "tags_id_seq" for serial column "tags.id"
	CREATE TABLE
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
	CREATE INDEX
```
  1. If everything went as expected in the previous steps, you should edit  tasti\_config.php to fill in all the variables as appropriate for your installation.
  1. You should now be ready to access **Tasti** in your web browser by pointing it to http://example.com/tasti  Assuming that you are able to Register a new user successfully, you should delete tasti.sql .

## HELP, and Contact Info ##

**Tasti** is officially hosted via this Google Code website.

Please submit bugs or feature requests above.  If you are experiencing problems, or have questions, I can be contacted by emailing netllama+tasti@gmail.com .  I'll try to respond within 24 hours.