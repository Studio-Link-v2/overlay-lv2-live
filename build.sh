#!/bin/bash -e

gcc -g -Wall -shared -o studio-link-live.so -fPIC studio-link-live.c ../my_include/libbaresip_live.a \
    ../re/libre.a ../rem/librem.a ../opus/libopus.a -I../my_include \
    ../openssl/libssl.a ../openssl/libcrypto.a -lm -lpthread -lz -ldl -lresolv

ldd studio-link-live.so
