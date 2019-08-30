#!/usr/bin/env bash
PROGRAM=${1}
cd suchai-drivers-obc
python2 waf configure build $PROGRAM
#python2 waf configure build $1 --with-os freertos --enable-if-i2c --with-rtable cidr --disable-param-cmd
cd -
