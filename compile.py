#!/usr/bin/env python
import os
import sys
import argparse
import src.system.include.configure as configure

available_os = ["LINUX", "FREERTOS"]
available_archs = ["X86", "GROUNDSTATION", "RPI", "NANOMIND", "ESP32", "AVR32"]
available_tests = ['test_cmd', 'test_unit', 'test_load', 'test_bug_delay', 'test_sgp4']
available_test_archs = ["X86"]
available_log_lvl = ["LOG_LVL_NONE", "LOG_LVL_ERROR", "LOG_LVL_WARN", "LOG_LVL_INFO", "LOG_LVL_DEBUG", "LOG_LVL_VERBOSE"]

def get_parameters():
    """
    Parse script arguments
    """
    parser = argparse.ArgumentParser(prog='compile.py')
    # config.h template parameters
    parser.add_argument('os', type=str, default="LINUX", choices=available_os)
    parser.add_argument('arch', type=str, default="X86", choices=available_archs)
    parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO", choices=available_log_lvl)
    parser.add_argument('--name', type=str, default="SUCHAI-DEV")
    parser.add_argument('--id',   type=str, default="0")
    parser.add_argument('--version',   type=str, default=configure.call_git_describe())
    parser.add_argument('--con', type=str, default="1")
    parser.add_argument('--comm', type=str, default="1")
    parser.add_argument('--fp', type=str, default="1")
    parser.add_argument('--hk', type=str, default="1")
    parser.add_argument('--sen', type=str, default="0")
    parser.add_argument('--test', type=str, default="0")
    parser.add_argument('--node', type=str, default="1")
    parser.add_argument('--zmq_in', type=str, default="tcp://127.0.0.1:8001")
    parser.add_argument('--zmq_out', type=str, default="tcp://127.0.0.1:8002")
    parser.add_argument('--st_mode', type=str, default="1")
    parser.add_argument('--st_triple_wr', type=str, default="1")
    # Build parameters
    parser.add_argument('--drivers', action="store_true", help="Install platform drivers")
    parser.add_argument('--ssh', action="store_true", help="Use ssh for git clone")
    parser.add_argument('--test_type', type=str, default='', choices=available_tests)
    # Force clean
    parser.add_argument('--clean', action="store_true", help="Clean before build")
    # Program
    parser.add_argument('--program', action="store_true", help="Compile and program")
    parser.add_argument('--console', type=int, default=4, help="Console to use. 2=Nanomind-USB-SERIAL, 4=FFP-USB")
    # Skip config
    parser.add_argument('--no-config', action="store_true", help="Skip configure, do not generate a new config.h")

    return parser.parse_args()


if __name__ == "__main__":
    # Parse parameters
    args = get_parameters()

    cwd_root = os.getcwd()

    # Generate config file
    if not args.no_config:
        configure.make_config(args,
                              'src/system/include/config_template.h',
                              'src/system/include/config.h')

    result = 0

    # Build
    if args.os == "LINUX":
        arch = args.arch.lower()
        arch_dir = os.path.join("src/drivers", arch)
        build_dir = "build_{}".format(arch)

        # Install drivers
        if args.drivers:
            os.chdir(arch_dir)
            os.system('sh install.sh')
            os.chdir(cwd_root)
            
        # Run tests
        if args.test_type in available_tests and args.arch in available_test_archs:
            test_dir = os.path.join(cwd_root, "test", args.test_type, "build_test")
            os.system('rm -rf {}'.format(test_dir))
            os.system('mkdir {}'.format(test_dir))
            os.chdir(test_dir)
            os.system('cmake ..')
            result = os.system('make')
            # Run the test
            if result == 0:
                if args.test_type == 'test_cmd':
                    print('./SUCHAI_Flight_Software_Test > log.txt...')
                    os.system('./SUCHAI_Flight_Software_Test > log.txt')
                    os.system('cp -f log.txt ../test_cmd_log.txt')
                    os.chdir("..")
                    print('python3 logs_comparator.py...')
                    result = os.system('python3 logs_comparator.py')
                else:
                    result = os.system('./SUCHAI_Flight_Software_Test')
        # Build
        else:
            os.system('rm -rf {}'.format(build_dir))
            os.system('mkdir {}'.format(build_dir))
            os.chdir(build_dir)
            os.system('cmake {}'.format(os.path.join("..", arch_dir)))
            result = os.system('make')

    else:  # args.os = FREERTOS
        if args.arch == "ESP32":
            if args.drivers:
                os.chdir('src/drivers/esp32')
                os.system('sh install.sh')
                os.chdir(cwd_root)
            elif args.program:
                result = os.system('make flash')
            else:
                result = os.system('make')

        if args.arch == "AVR32":
            os.chdir('src/drivers/atmel')
            # Install ASF
            if args.drivers:
                os.system('sh install.sh')
            os.chdir('xdk-asf-3.33.0/avr32/applications/suchai/xplained/gcc')
            result = os.system('sh build.sh')
            os.chdir(cwd_root)

        if args.arch == "NANOMIND":
            os.chdir('src/drivers/nanomind')
            # Install Nanomind SDK and LibCSP
            if args.drivers:
                if not args.ssh:
                    result = os.system('sh install.sh')
                else:
                    result = os.system('sh install.sh --ssh')
            elif args.clean:
                result = os.system('sh build.sh clean')
            elif args.program:
                result = os.system('sh build.sh program' + ' ' + str(args.console))
            else:
                result = os.system('sh build.sh')

    if result != 0:
        sys.exit(1)

