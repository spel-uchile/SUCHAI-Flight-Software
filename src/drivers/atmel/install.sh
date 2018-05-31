#!/bin/sh

echo "Downloading toolchain..."
wget -N data.spel.cl/atmel.tar.gz
tar -k -xzf atmel.tar.gz

echo "Linking AVR32 headers..."
cd avr32-gnu-toolchain-linux_x86_64/avr32/include/
ln -s -f ../../../avr32-headers-6.2.0.742/avr32/
cd -

echo "Fixing headers"
# Fix SDRAM include
sed -i "s/#define SDRAM_PART_HDR              \"mt48lc16m16a2tg7e\/mt48lc16m16a2tg7e.h\"/#define SDRAM_PART_HDR              \"mt48lc4m16a2tg7e.h\"/g" xdk-asf-3.33.0/avr32/boards/uc3_a3_xplained/uc3_a3_xplained.h
sed -i "s/#define configTICK_TC_IRQ             ATPASTE2(AVR32_TC_IRQ, configTICK_TC_CHANNEL)/#define configTICK_TC_IRQ             ATPASTE2(AVR32_TC0_IRQ, configTICK_TC_CHANNEL)/g" xdk-asf-3.33.0/thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/portmacro.h

echo "Coping custom ASF application..."
cp -r suchai/ xdk-asf-3.33.0/avr32/applications/

echo "Linking source code to ASF application..."
cd xdk-asf-3.33.0/avr32/applications/suchai/
ln -s -f ../../../../../../system
ln -s -f ../../../../../../os
echo "Linking libcsp..."
ln -s -f ../../../../../../drivers/atmel/libcsp
cd -

echo "Moving the FreeRTOS configuration...."
cp FreeRTOSConfig.h xdk-asf-3.33.0/thirdparty/freertos/freertos-7.0.0/module_config/FreeRTOSConfig.h

echo "---------"
echo "Finished. Compile and program with:"
echo "sh build.sh"
echo "sh build.sh dfu"
echo "---------"
