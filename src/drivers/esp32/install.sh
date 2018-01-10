#!/bin/sh

echo "Downloading toolchain..."
#wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz
wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-75-gbaf03c2-5.2.0.tar.gz
#tar -xzf xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-linux64-1.22.0-75-gbaf03c2-5.2.0.tar.gz

echo "Downloading ESP-IDF...."
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git submodule update --init
cd ..

echo "Fixing FreeRTOS includes..."
sed -i "s/COMPONENT_ADD_INCLUDEDIRS := include/COMPONENT_ADD_INCLUDEDIRS := include include\/freertos/g" esp-idf/components/freertos/component.mk


echo "Moving files to corrects directions"
mv component-main.mk ~/SUCHAI-Flight-Software/src/system/component.mk
mv component-freertos.mk ~/SUCHAI-Flight-Software/src/os/FreeRTOS/component.mk
mv Makefile ~/SUCHAI-Flight-Software/Makefile
mv sdkconfig ~/SUCHAI-Flight-Software/sdkconfig


echo "---------"
echo "Finished"
echo "Add the following lines to your bash.rc file"
echo "export PATH=\$PATH:$(pwd)/xtensa-esp32-elf/bin"
echo "export IDF_PATH=$(pwd)/esp-idf"
echo "---------"
