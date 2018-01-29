#!/usr/bin/env bash
echo "Downloading libcsp v1.4"
[ ! -e libcsp ] && git clone https://github.com/libcsp/libcsp 

cd libcsp
git pull origin master
cd ..

echo "Copy csp config for FreeRTOS"
[ -e csp_config.h  ] && cp -rf csp_config.h include/csp/csp_autoconfig.h

#echo $PATH
#echo "Build libcsp"
#./waf configure --with-os=freertos  --enable-if-i2c  --install-csp --prefix=../ build install

cd -
