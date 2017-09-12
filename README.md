# ARN Server [![Build Status](https://travis-ci.org/Arenomai/server.svg?branch=master)](https://travis-ci.org/Arenomai/server)

Server repository.

## Test Server
A test server is running at this address : thgros.eu port : 4242

This test server is intended to be used with the current Arenomai 
version, from this repository.

## Dependencies

- `libpq` (PostgreSQL C implementation)
- `gcc` 6+
- `cmake` 2.8+



### Installing dependencies

#### Debian / Ubuntu

*Make sure you have the latest version of your distribution*

```
sudo apt update
sudo apt install libpq-dev gcc-6 cmake
```

#### Arch Linux

`sudo pacman -S sed make gcc cmake postgresql`


## Database setup

Make sure your database is running and working normally.

#### 1 - Users

You'll need a valid user.
We highly recommend you to create a dedicated user to the *Arenomai* server, for a better security.

#### 2 - Tables

We currently use 2 tables :

  *accounts :*
  - nickname (text)
  - bio (text)
  - token (integer unique non null primary key)


  *users :*
  - token (serial primary key)
  - username (text unique non null)
  - password (text non null)

Table creation commands :

`CREATE TABLE accounts (nickname text, bio text, token integer unique primary key);`  

`CREATE TABLE users (token serial primary key, username text unique  not null,password text not null);`

#### 3 - Properties file

At the server root (where the *arenomai* binary is located) a file called properties.txt is needed. An exemple is located in the project root.  

**Exemple :**  

```
hostaddr=127.0.0.1
port=5432
dbname=arenomai
user=arenomai
password=arenomai
```



**hostaddr** is the ip address of your PostgreSQL server.  
**port** is the port of your PostgreSQL server.  
**dbname** is the database name.  
**user** is the PostgreSQL user with the access to the database.  
**password** is the PostgreSQL user's database password (this password should'nt be the same as the UNIX user password).  
