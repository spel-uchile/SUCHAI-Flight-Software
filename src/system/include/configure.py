#!/usr/bin/env python

import sys, argparse

parser = argparse.ArgumentParser(prog='configure.py')
parser.add_argument('os', type=str, default="LINUX")
parser.add_argument('--arch', type=str, default="ESP32")
parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO")
parser.add_argument('--sch_comm', type=int, default=1)
parser.add_argument('--sch_fp', type=int, default=1)
parser.add_argument('--sch_hk', type=int, default=1)
parser.add_argument('--sch_test', type=int, default=0)
parser.add_argument('--sch_st_mode', type=int, default=1)

args = parser.parse_args()

with open('config_template.h', 'r') as config:
    config = config.read()

config = config.replace("{{OS}}", args.os)
config = config.replace("{{ARCH}}", args.arch)
config = config.replace("{{LOG_LVL}}", args.log_lvl)
config = config.replace("{{SCH_COMM}}", args.sch_comm)
config = config.replace("{{SCH_FP}}", args.sch_fp)
config = config.replace("{{SCH_HK}}", args.sch_hk)
config = config.replace("{{SCH_TEST}}", args.sch_test)
config = config.replace("{{SCH_ST_MODE}}", args.sch_st_mode)

with open('config.h', 'w') as new_config:
    new_config.write(config)