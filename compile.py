#!/usr/bin/env python
import argparse
import os

import src.system.include.configure as configure

def get_parameters():
    """
    Parse script arguments
    """
    parser = argparse.ArgumentParser(prog='compile.py')
    # config.h template parameters
    parser.add_argument('os', type=str, default="LINUX")
    parser.add_argument('--arch', type=str, default="ESP32")
    parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO")
    parser.add_argument('--sch_comm', type=str, default="1")
    parser.add_argument('--sch_fp', type=str, default="1")
    parser.add_argument('--sch_hk', type=str, default="1")
    parser.add_argument('--sch_test', type=str, default="0")
    parser.add_argument('--sch_node', type=str, default="1")
    parser.add_argument('--sch_zmq_out', type=str, default="tcp://127.0.0.1:8001")
    parser.add_argument('--sch_zmq_in', type=str, default="tcp://127.0.0.1:8002")
    parser.add_argument('--sch_st_mode', type=str, default="1")
    # Build parameters
    parser.add_argument('--drivers', action="store_true", help="Install platform drivers")

    return parser.parse_args()

if __name__ == "__main__":
    # Parse parameters
    args = get_parameters()

    # Generate config file
    cwd_root = os.getcwd()
    configure.make_config(args,
                          'src/system/include/config_template.h',
                          'src/system/include/config.h')

    result = 0

    # Build
    if args.os == "LINUX":
        # Install CSP drivers
        if args.drivers:
            os.chdir('src/drivers/Linux/libcsp')
            os.system('sh install_csp.sh')
            os.chdir(cwd_root)

        os.system('rm -rf build_linux')
        os.system('mkdir build_linux')
        os.chdir('build_linux')
        os.system('cmake ..')
        result = os.system('make')

    else: #args.os = FREERTOS
        if args.arch == "ESP32":
            #TODO: Install IDF drivers
            system = os.system('make')

        else: #args.arch = AVR32
            os.chdir('src/drivers/atmel')
            # Install ASF
            if args.drivers:
                os.system('sh install.sh')
            os.chdir('xdk-asf-3.33.0/avr32/applications/suchai/xplained/gcc')
            result = os.system('sh build.sh')
            os.chdir(cwd_root)

    exit(result)