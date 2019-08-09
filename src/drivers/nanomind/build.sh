#!/usr/bin/env bash
PROGRAM=${1}
CONSOLE=${2:-4}
cd a3200-sdk-lite-v1.2/
python2 waf configure build $PROGRAM --with-os freertos --enable-if-i2c --with-rtable cidr --disable-param-cmd --with-console $CONSOLE
#python2 waf configure build $1 --with-os freertos --enable-if-i2c --with-rtable cidr --disable-param-cmd
cd -
