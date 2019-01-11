#!/usr/bin/env bash

# Script for automated test running.
#
# The results of each test can be found on log files that generate after execution.
#
# These logs will generate inside each test's directory.
#
# Authors:  Tamara Gutierrez R.
#           Diego Ortego P.

# Gets the current execution directory (the absolute path to this script)
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
export WORKSPACE=${SCRIPT_PATH}/..

# Prints the current workspace path, for debugging purposes
echo ${WORKSPACE}

# ------------------ TEST_CMD ------------------

# The main test log is called test_cmd_log.txt
# That log is compares with test_cmd_log_base.txt
# The result of the comparison is in test_cmd_comparator_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --sch_comm "0" --sch_fp "0" --sch_hk "0" --sch_test "1" --sch_st_mode "0"

# Compiles the test
cd ${WORKSPACE}/test/test_cmd
rm -rf build_test
mkdir build_test
cd build_test
cmake ..
make

# Runs the test, saving a log file
rm -f ../test_cmd_log.txt
./SUCHAI_Flight_Software_Test | cat >> ../test_cmd_log.txt
echo ""

# Makes result comparison
cd ..
rm -f test_cmd_comparator_log.txt
python logs_comparator.py | cat >> test_cmd_comparator_log.txt

# ------------------ TEST_UNIT ------------------

# TODO: Agregar for loop con el test en todos los storage modes

# The test log is called test_unit_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --sch_comm "0" --sch_fp "0" --sch_hk "0" --sch_test "0" --sch_st_mode "1"

# Compiles the test
cd ${WORKSPACE}/test/test_unit
rm -rf build_test
mkdir build_test
cd build_test
cmake ..
make

# Runs the test, saving a log file
rm -f ../test_unit_log.txt
./SUCHAI_Flight_Software_Test | cat >> ../test_unit_log.txt
echo ""

# ------------------ TEST_LOAD ------------------

# The test log is called test_load_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --sch_comm "0" --sch_fp "0" --sch_hk "0" --sch_test "0" --sch_st_mode "0"

# Compiles the test
cd ${WORKSPACE}/test/test_load
rm -rf build_test
mkdir build_test
cd build_test
cmake ..
make

# Runs the test, saving a log file
rm -f ../test_load_log.txt
./SUCHAI_Flight_Software_Test | cat >> ../test_load_log.txt
echo ""

# ------------------ TEST_BUG_DELAY ------------------

# The test log is called test_bug_delay_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --sch_comm "0" --sch_fp "0" --sch_hk "0" --sch_test "0" --sch_st_mode "0"

# Compiles the test
cd ${WORKSPACE}/test/test_bug_delay
rm -rf build_test
mkdir build_test
cd build_test
cmake ..
make

# Runs the test, saving a log file
rm -f ../test_bug_delay_log.txt
./SUCHAI_Flight_Software_Test | cat >> ../test_bug_delay_log.txt
