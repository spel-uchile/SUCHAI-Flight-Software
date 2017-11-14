#!/usr/bin/env bash
echo "Downloading libcsp"
[ ! -e libcsp ] && git clone git@github.com:libcsp/libcsp.git
cd libcsp

echo "Build libcsp"
./waf configure --with-os=posix --enable-if-zmqhub --install-csp --prefix=../ build install

cd -