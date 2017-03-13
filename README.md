# ARN Server [![Build Status](https://travis-ci.org/Arenomai/server.svg?branch=master)](https://travis-ci.org/Arenomai/server)

Server repository.

Dependancies :

```
libpq (PostgreSQL C implementation)
gcc-6
cmake
```

Installing the dependancies :

**DEBIAN/UBUNTU**

*Make sure you have the latest version of your distribution*

```
sudo apt update
sudo apt install libpq-dev gcc-6 cmake
```

**Archlinux**

`sudo pacman -S libpqxx gcc cmake`

Working implementations :
    - Database connection.
    - TCP connection. 
    - MessagePack.
    - Auth.
