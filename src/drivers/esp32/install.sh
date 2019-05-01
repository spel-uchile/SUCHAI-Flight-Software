#!/bin/sh

echo "Downloading toolchain..."
wget -N https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

echo "Downloading ESP-IDF...."
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git submodule update --init
cd ..

echo "Moving files to corrects directions"
cp component-main.mk ../../../src/system/component.mk
cp component-freertos.mk ../../../src/os/FreeRTOS/component.mk
cp Makefile ../../../Makefile
cp sdkconfig ../../../sdkconfig


echo "---------"
echo "Finished"
echo "Add the following lines to your bash.rc file"
echo "export PATH=\$PATH:$(pwd)/xtensa-esp32-elf/bin"
echo "export IDF_PATH=$(pwd)/esp-idf"
echo "---------"
