#!/usr/bin/env python

import sys, argparse, os

parser = argparse.ArgumentParser(prog='compile.py')
parser.add_argument('os', type=str, default="LINUX")
parser.add_argument('--arch', type=str, default="ESP32")
parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO")
parser.add_argument('--sch_comm', type=str, default="1")
parser.add_argument('--sch_fp', type=str, default="1")
parser.add_argument('--sch_hk', type=str, default="1")
parser.add_argument('--sch_test', type=str, default="0")
parser.add_argument('--sch_st_mode', type=str, default="1")

args = parser.parse_args()

os.chdir('src/system/include')
os.system('python configure.py ' + args.os + " --arch " + args.arch + " --log_lvl " + args.log_lvl + " --sch_comm " + args.sch_comm
          + " --sch_fp " + args.sch_fp + " --sch_hk " + args.sch_hk + " --sch_test " + args.sch_test + " --sch_st_mode " + args.sch_st_mode)

if args.os == "LINUX":
    os.chdir('../../drivers/Linux/libcsp')
    os.system('sh install_csp.sh')
    os.chdir('../../../..')

    os.system('rm -rf build_linux')
    os.system('mkdir build_linux')
    os.chdir('build_linux')
    os.system('cmake ..')
    os.system('make')

else: #args.os = FREERTOS
    if args.arch == "ESP32":
        os.chdir('../../..')
        os.system('make')
    else: #args.arch = AVR32
        os.chdir('../../drivers/atmel')
        os.system('sh install.sh')
        os.chdir('xdk-asf-3.33.0/avr32/applications/suchai/xplained/gcc')
        os.system('sh build.sh')
