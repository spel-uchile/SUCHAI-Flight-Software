"""
Simple script that compares the results of 8 tests between a base output
and the current one. It counts the number of command results equal to 1
for each file, then it compares those numbers.
"""

__author__ = "Tamara Gutierrez R, Diego Ortego P"

import re

def compare():
    p = re.compile('Command result: 1')

    with open('test_cmd_log_base.txt', 'r') as myfile1:
        logfile1=myfile1.read()

    with open('test_cmd_log.txt', 'r') as myfile2:
        logfile2=myfile2.read()

    if len(p.findall(logfile1)) != len(p.findall(logfile2)):
        print("[Logs Comparator] Unsuccessful execution")
        exit(1)
    print("[Logs Comparator] Successful execution")
    exit(0)

compare()
