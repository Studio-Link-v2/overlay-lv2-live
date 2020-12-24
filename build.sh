#!/bin/bash -e

gcc -g -Wall -shared -o studio-link-onair.so -fPIC studio-link-onair.c ../my_include/libbaresip_onair.a \
    ../re/libre.a ../rem/librem.a ../3rdparty/lib/libopus.a -I../my_include \
     -lm -lpthread -lz -ldl -lresolv -lcrypt -lssl

ldd studio-link-onair.so
