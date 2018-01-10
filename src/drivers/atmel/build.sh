#!/usr/bin/env bash
cd xdk-asf-3.33.0/avr32/applications/suchai/xplained/gcc
OPTION=${1:-""}
AVR32_PATH=$(realpath ../../../../../../avr32-gnu-toolchain-linux_x86_64)

export PATH=$PATH:${AVR32_PATH}/bin:${AVR32_PATH}/avr32/bin
export ASF_PATH=../../../../../../xdk-asf-3.33.0
make ${OPTION}
cd -
