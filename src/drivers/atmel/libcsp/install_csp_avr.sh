#!/usr/bin/env bash
echo "Downloading Gomspace-libcsp v1.5"
if [ ! -d "./libcsp" ]; then
    git clone https://github.com/GomSpace/libcsp.git
    cd libcsp
    git checkout 1.5.16
    cd -
fi

echo "Copy csp config for FreeRTOS"
[ -e csp_config.h  ] && cp -rf csp_config.h include/csp/csp_autoconfig.h

cd -
