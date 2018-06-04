#!/bin/sh
echo "Downloading toolchain..."
wget -N data.spel.cl/gs-avr32-toolchain-3.4.2.tar.gz
tar -xzf gs-avr32-toolchain-3.4.2.tar.gz
cd gs-avr32-toolchain-3.4.2
echo "Installing toolchain..."
sh install-avr32.sh
cd -

echo "Downloading Gomspace SDK..."
wget -N data.spel.cl/gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2
tar -xjf gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2

echo "Linking source code into SDK..."
cd a3200-sdk-lite-v1.2/
mv src/ src.old/
ln -s -f ../../../../src
mv wscript wscript.old
cd -

echo "Coping custom waf script..."
cp wscript a3200-sdk-lite-v1.2/wscript

echo "Adding libcsp to sdk lib folder"
if [ ! -d "./libcsp" ]; then
    git clone https://github.com/libcsp/libcsp
fi
cp -f libcsp_wscript ./libcsp/wscript
cd a3200-sdk-lite-v1.2/lib/
ln -s -f ../../libcsp
cd -

echo "---------"
echo "Finished. Compile and program with:"
echo "sh build.sh"
echo "sh build.sh program"
echo "---------"
