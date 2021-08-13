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
export WORKSPACE=${SCRIPT_PATH}

# Prints the current workspace path, for debugging purposes
echo ${WORKSPACE}

# ---------------- --TEST_UNIT ------------------

# The test log is called test_unit_log.txt

# Tests for all storage modes
cd ${WORKSPACE}/test_unit
rm -rf build_test
ST_TRIPLE_WR="1"

for ST_MODE in "RAM" "SQLITE"
do
    echo "Unit Test for storage ${ST_MODE}"

    # build the unitest with the test's parameters
    cmake -B build_test -DSCH_ST_MODE=${ST_MODE} -DSCH_STORAGE_TRIPLE_WR=${ST_TRIPLE_WR}
    cmake --build build_test

    # Runs the test, saving a log file
    cd build_test
    rm -f test_unit_${ST_MODE}_log.txt
    ./suchai-unitest #| cat >> test_unit_${ST_MODE}_log.txt
    echo ""
    cd -
done
cd ${WORKSPACE}

#TODO: DELETE THIS EXIT AFTER FIXING THE REST OF THE TESTS!
exit
#TODO: REMEMBER TO DELETE THIS EXIT!

# ---------------- --TEST_CMD ------------------

# The main test log is called test_cmd_log.txt
# That log is compares with test_cmd_log_base.txt
# The result of the comparison is in test_cmd_comparator_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --log_lvl "LOG_LVL_NONE"  --comm "0"  --fp "0"  --hk "0"  --test "1"  --st_mode "0"

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

# ---------------- --TEST_LOAD ------------------

# The test log is called test_load_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --log_lvl "LOG_LVL_NONE"  --comm "0"  --fp "0"  --hk "0"  --test "0"  --st_mode "0"

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

# ---------------- --TEST_BUG_DELAY ------------------

# The test log is called test_bug_delay_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --log_lvl "LOG_LVL_INFO"  --comm "0"  --fp "0"  --hk "0"  --test "0"  --st_mode "0"

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

# ---------------- --TEST_TM_IO ------------------

# The test log is called test_tm_io_log.txt

# Compiles the project with the test's parameters
cd ${WORKSPACE}/src/system/include
python3 configure.py "LINUX" --log_lvl "LOG_LVL_NONE"  --comm "1" --con "0" --fp "0"  --hk "0"  --test "0"  --st_mode "0"  --node "1"

# Compiles the test
cd ${WORKSPACE}/test/test_tm_io
rm -rf build_test
mkdir build_test
cd build_test
cmake ..
make

# Runs the test, saving a log file
rm -f ../test_tm_io_log.txt
./SUCHAI_Flight_Software_Test | cat >> ../test_tm_io_log.txt