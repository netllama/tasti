CREATE table users (id serial NOT NULL, username text NOT NULL UNIQUE, password text NOT NULL, created timestamp NOT NULL default now(), name text, email text NOT NULL) ;
CREATE index users_id_idx ON users(id) ;
CREATE index users_username_idx ON users(username) ;
CREATE index users_password_idx ON users(password) ;
CREATE index users_created_idx ON users(created) ;
CREATE index users_name_idx ON users(name) ;
CREATE index users_email_idx ON users(email) ;

CREATE table bmarks (id serial NOT NULL, created timestamp NOT NULL default now(), last_update timestamp NOT NULL default now(), owner text NOT NULL REFERENCES users(username), url text NOT NULL, name text NOT NULL, notes text, tag text) ;
CREATE index bmarks_id_idx ON bmarks (id) ;
CREATE index bmarks_created_idx ON bmarks (created) ;
CREATE index bmarks_last_update_idx ON bmarks (last_update) ;
CREATE index bmarks_owner_idx ON bmarks (owner) ;
CREATE index bmarks_url_idx ON bmarks (url) ;
CREATE index bmarks_notes_idx ON bmarks (notes) ;
CREATE index bmarks_tag_idx ON bmarks (tag) ;
CREATE index bmarks_name_idx ON bmarks (name) ;
CREATE UNIQUE index bmarks_owner_url_tag_idx ON bmarks(owner,url,tag) ;

CREATE table tags (id serial NOT NULL, created timestamp NOT NULL default now(), last_update timestamp NOT NULL default now(), owner text NOT NULL references users(username), tag text NOT NULL) ;
CREATE index tags_id_idx ON tags (id) ;
CREATE index tags_created_idx ON tags (created) ;
CREATE index tags_last_update_idx ON tags (last_update) ;
CREATE index tags_owner_idx ON tags (owner) ;
CREATE index tags_tag_idx ON tags (tag) ;
CREATE UNIQUE index tags_owner_tag_idx ON tags (owner,tag) ;
