#!/usr/bin/env python
import argparse
from subprocess import Popen, PIPE
import os


def call_git_describe(abbrev=4):
    try:
        p = Popen(['git', 'describe', '--abbrev=%d' % abbrev],
                  stdout=PIPE, stderr=PIPE)
        p.stderr.close()
        line = p.stdout.readlines()[0]
        return line.strip().decode("utf-8")
    except:
        return "0.0.0"


def parse_args():
    """
    Parse console arguments
    :return: arguments dict
    """
    parser = argparse.ArgumentParser(prog='configure.py')
    parser.add_argument('os', type=str, default="LINUX")
    parser.add_argument('--arch', type=str, default="X86")
    parser.add_argument('--log_lvl', type=str, default="LOG_LVL_INFO")
    parser.add_argument('--name', type=str, default="SUCHAI-DEV")
    parser.add_argument('--id',   type=str, default="0")
    parser.add_argument('--version',   type=str, default=call_git_describe())
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

    args = parser.parse_args()
    return args


def make_config(args, ftemp="config_template.h", fconfig="config.h"):
    """
    Write config file from template
    :param args: arguments dict, from @parse_args
    :param ftemp: template file path
    :param fconfig: output file path
    :return: None
    """
    with open(ftemp, 'r') as config:
        config = config.read()

    config = config.replace("{{OS}}", args.os)
    config = config.replace("{{ARCH}}", args.arch)
    config = config.replace("{{LOG_LVL}}", args.log_lvl)
    config = config.replace("{{NAME}}", args.name)
    config = config.replace("{{ID}}", args.id)
    config = config.replace("{{VERSION}}", args.version)
    config = config.replace("{{SCH_EN_CON}}", args.con)
    config = config.replace("{{SCH_EN_COMM}}", args.comm)
    config = config.replace("{{SCH_EN_FP}}", args.fp)
    config = config.replace("{{SCH_EN_HK}}", args.hk)
    config = config.replace("{{SCH_EN_TEST}}", args.test)
    config = config.replace("{{SCH_EN_SEN}}", args.sen)
    config = config.replace("{{SCH_COMM_NODE}}", args.node)
    config = config.replace("{{SCH_ZMQ_OUT}}", args.zmq_out)
    config = config.replace("{{SCH_ZMQ_IN}}", args.zmq_in)
    config = config.replace("{{SCH_STORAGE}}", args.st_mode)
    config = config.replace("{{SCH_STORAGE_TRIPLE_WR}}", args.st_triple_wr)
    config = config.replace("{{SCH_STORAGE_PGUSER}}", os.environ['USER'])

    with open(fconfig, 'w') as new_config:
        new_config.write(config)


if __name__ == "__main__":
    make_config(parse_args())
