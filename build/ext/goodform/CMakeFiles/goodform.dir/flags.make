# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# compile CXX with /usr/bin/c++
CXX_FLAGS =    -D'htonll(x)=((1==htonl(1))?(x):((uint64_t)htonl((x)&0xFFFFFFFF)<<32)|htonl((x)>>32))' -D'ntohll(x)=((1==ntohl(1))?(x):((uint64_t)ntohl((x)&0xFFFFFFFF)<<32)|ntohl((x)>>32))' -std=c++11 -fPIC  

CXX_DEFINES = -Dgoodform_EXPORTS

CXX_INCLUDES = -I/home/krevar/Cours/S3/PTUT/projet/server/ext/goodform/./include 

