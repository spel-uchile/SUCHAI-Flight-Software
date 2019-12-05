#!/usr/bin/env bash

#From: https://docs.espressif.com/projects/esp-idf/en/stable/get-started/linux-setup.html
echo "Downloading toolchain..."
wget -N https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

#From: https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html

echo "Downloading ESP-IDF v3.3...."
if [ ! -d "./esp-idf" ]; then
    git clone -b v3.3 --recursive https://github.com/espressif/esp-idf.git
fi
#cd esp-idf
#git submodule update --init
#cd ..

echo "Installing python requirements"
python2 -m pip install --user -r esp-idf/requirements.txt

echo "Moving files to corrects directions"
cp component-main.mk ../../../src/system/component.mk
cp component-freertos.mk ../../../src/os/FreeRTOS/component.mk
cp Makefile ../../../Makefile
cp sdkconfig ../../../sdkconfig


echo "---------"
echo "Finished"
echo "Add the following lines to your bash.rc or .profile file"
echo "export PATH=\$PATH:$(pwd)/xtensa-esp32-elf/bin"
echo "export IDF_PATH=$(pwd)/esp-idf"
#echo "To set up environment variables run"
#echo ". esp-idf/export.sh"
echo "---------"
