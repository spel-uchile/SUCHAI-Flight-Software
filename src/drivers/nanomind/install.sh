#!/bin/sh
echo "Downloading toolchain..."
wget -N api.spel.cl/gs-avr32-toolchain-3.4.2.tar.gz
tar -xzf gs-avr32-toolchain-3.4.2.tar.gz
cd gs-avr32-toolchain-3.4.2
echo "Installing toolchain..."
if [ ! -d "~/.local" ]; then
    mkdir ~/.local
fi
if [ ! -d "~/.local/avr32" ]; then
    mkdir ~/.local/avr32
fi
if [ ! -d "~/.local/bin" ]; then
    mkdir ~/.local/bin
fi
if [ ! -d "~/.local/share" ]; then
    mkdir ~/.local/share
fi
./install-avr32.sh
cd -

echo "Downloading Gomspace SDK..."
wget -N api.spel.cl/gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2
tar -xjf gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2

echo "Linking source code into SDK..."
cd a3200-sdk-lite-v1.2/
mv src/ src.old/
ln -s -f ../../../../src
mv wscript wscript.old
cd -

echo "Coping custom waf script..."
cp wscript a3200-sdk-lite-v1.2/wscript
echo "Coping custom syscalls_basic.c..."
cp -f syscalls_basic.c a3200-sdk-lite-v1.2/lib/libasf/gomspace/freertos/avr32/

echo "Adding libcsp to sdk lib folder"
if [ ! -d "./libcsp" ]; then
    git clone https://github.com/libcsp/libcsp
fi
cp -f libcsp_wscript ./libcsp/wscript
cd a3200-sdk-lite-v1.2/lib/
ln -s -f ../../libcsp
cd -

echo "---------"
echo "Dont forget to add the following to your .bashrc file"
echo "export PATH=\"\~/.local/bin:\$PATH\""
echo "export PATH=\"\~/.local/avr32/bin:\$PATH\""
echo "---------"
echo "Finished. Compile and program with:"
echo "sh build.sh"
echo "sh build.sh program"
echo "---------"
