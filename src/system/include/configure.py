#!/usr/bin/env python
import argparse

def parse_args():
    """
    Parse console arguments
    :return: arguments dict
    """
    parser = argparse.ArgumentParser(prog='configure.py')
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
    config = config.replace("{{SCH_EN_COMM}}", args.sch_comm)
    config = config.replace("{{SCH_EN_FP}}", args.sch_fp)
    config = config.replace("{{SCH_EN_HK}}", args.sch_hk)
    config = config.replace("{{SCH_EN_TEST}}", args.sch_test)
    config = config.replace("{{SCH_COMM_NODE}}", args.sch_node)
    config = config.replace("{{SCH_ZMQ_OUT}}", args.sch_zmq_out)
    config = config.replace("{{SCH_ZMQ_IN}}", args.sch_zmq_in)
    config = config.replace("{{SCH_STORAGE}}", args.sch_st_mode)

    with open(fconfig, 'w') as new_config:
        new_config.write(config)

if __name__ == "__main__":
    make_config(parse_args())