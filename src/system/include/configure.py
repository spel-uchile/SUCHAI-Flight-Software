#!/usr/bin/env python

import sys, argparse

parser = argparse.ArgumentParser(prog='configure.py')
parser.add_argument('os', type=str, default="LINUX")
parser.add_argument('--arch', type=str, default="ESP32")
parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO")

args = parser.parse_args()

with open('config_template.h', 'r') as config:
    config = config.read()

config = config.replace("{{OS}}", args.os)
config = config.replace("{{ARCH}}", args.arch)
config = config.replace("{{LOG_LVL}}", args.log_lvl)

with open('config.h', 'w') as new_config:
    new_config.write(config)