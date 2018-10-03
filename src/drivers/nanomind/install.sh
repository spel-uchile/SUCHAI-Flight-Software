#!/bin/sh
# echo "Downloading toolchain..."
# wget -N data.spel.cl/gs-avr32-toolchain-3.4.2.tar.gz
# tar -xzf gs-avr32-toolchain-3.4.2.tar.gz
# cd gs-avr32-toolchain-3.4.2
# echo "Installing toolchain..."
# if [ ! -d "~/.local" ]; then
#     mkdir ~/.local
# fi
# if [ ! -d "~/.local/avr32" ]; then
#     mkdir ~/.local/avr32
# fi
# if [ ! -d "~/.local/bin" ]; then
#     mkdir ~/.local/bin
# fi
# if [ ! -d "~/.local/bin" ]; then
#     mkdir ~/.local/share
# fi
# ./install-avr32.sh
# cd -

echo "Downloading Gomspace SDK..."
wget -N data.spel.cl/gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2
tar -xjf gs-sw-nanomind-a3200-sdk-lite-v1.2.tar.bz2

echo "Linking source code into SDK..."
cd a3200-sdk-lite-v1.2/
mv src/ src.old/
ln -s -f ../../../../src
cd -

echo "Coping custom waf script..."
mv a3200-sdk-lite-v1.2/wscript a3200-sdk-lite-v1.2/wscript.old
cp wscript a3200-sdk-lite-v1.2/wscript

echo "Custom i2c.h..."
mv a3200-sdk-lite-v1.2/lib/libasf/gomspace/drivers/include/dev/i2c.h a3200-sdk-lite-v1.2/lib/libasf/gomspace/drivers/include/dev/i2c.h.old
cp i2c.h.suchai a3200-sdk-lite-v1.2/lib/libasf/gomspace/drivers/include/dev/i2c.h

echo "Custom syscallbasic..."
mv a3200-sdk-lite-v1.2/lib/libasf/gomspace/freertos/avr32/syscalls_basic.c a3200-sdk-lite-v1.2/lib/libasf/gomspace/freertos/avr32/syscalls_basic.c.old
cp syscalls_basic.c.suchai a3200-sdk-lite-v1.2/lib/libasf/gomspace/freertos/avr32/syscalls_basic.c

echo "Adding libcsp v1.4 to sdk lib folder"
if [ ! -d "./libcsp" ]; then
    git clone https://github.com/libcsp/libcsp
    cd libcsp
    git checkout v1.4
    echo "Replacing csp drivers..."
    mv include/csp/drivers/ include/csp/drivers.bak
    sed -i "s/#include <csp\/drivers\/i2c.h>/#include <dev\/i2c.h>/g" src/interfaces/csp_if_i2c.c
    sed -i "s/#include <csp\/drivers\/i2c.h>/#include <dev\/i2c.h>/g" include/csp/interfaces/csp_if_i2c.h
    # I had a bug in arch: using sed the file loss permissions
    chmod 644 src/interfaces/csp_if_i2c.c
    chmod 644 include/csp/interfaces/csp_if_i2c.h
    cd -
fi
cd a3200-sdk-lite-v1.2/lib/
ln -s -f ../../libcsp
cd -

echo "Downloading GomSpace drivers..."
wget -N data.spel.cl/gs-drivers.tar.gz
tar -xzf gs-drivers.tar.gz
echo "Adding GomSpace drivers..."
cd a3200-sdk-lite-v1.2/lib/
ln -s -f ../../gs-drivers/libparam
cd -

#echo "---------"
#echo "Do not forget to add the following to your .bashrc file"
#echo "export PATH=\"\~/.local/bin:\$PATH\""
#echo "export PATH=\"\~/.local/avr32/bin:\$PATH\""

echo "---------"
echo "Finished. Compile and program with:"
echo "sh build.sh"
echo "sh build.sh program"
echo "---------"
