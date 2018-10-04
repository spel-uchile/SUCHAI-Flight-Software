#!/usr/bin/env bash
echo "Downloading libcsp v1.5.dev"
if [ ! -d "./libcsp" ]; then
    git clone https://github.com/libcsp/libcsp
    cd libcsp
    git checkout release-1.5
    mv wscript wscript.old
    cd -
    cp wscript libcsp/wscript
fi

cd libcsp
echo "Build libcsp"
python2 ./waf configure --with-os=posix --enable-if-zmqhub --enable-if-kiss --enable-crc32 --with-driver-usart=linux --install-csp --prefix=../ build install
cd -
