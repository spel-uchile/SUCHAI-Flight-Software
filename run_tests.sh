#!/usr/bin/env bash
# Script for automated test running.
#
# The results of each test can be found on log files that generate after execution.
#
# These logs will generate inside each test's directory.
#
# Authors:  Tamara Gutierrez R.
#           Diego Ortego P.
#           Carlos Gonzalez C.

test_build_enabled=1
test_unit_enabled=1
test_cmd_enabled=1
test_bug_delay_enabled=0
test_load_enabled=1
test_sgp4_enabled=1
test_tm_io_enabled=1
test_file_enabled=0

while getopts ":b:u:c:d:l:s:t:f:h" opt; do
    case ${opt} in
        b )
            test_build_enabled=$OPTARG
            ;;
        u )
            test_unit_enabled=$OPTARG
            ;;
        c )
            test_cmd_enabled=$OPTARG
            ;;
        d )
            test_bug_delay_enabled=$OPTARG
            ;;
        l )
            test_load_enabled=$OPTARG
            ;;
        s )
            test_sgp4_enabled=$OPTARG
            ;;
        t )
            test_tm_io_enabled=$OPTARG
            ;;
        f )
            test_file_enabled=$OPTARG
            ;;
        \? )
            echo "Invalid option: $OPTARG" 1>&2
            ;;
        : )
            echo "Invalid option: $OPTARG requires an argument" 1>&2
            ;;
    esac
done
shift $((OPTIND -1))

# Gets the current execution directory (the absolute path to this script)
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
WORKSPACE=${SCRIPT_PATH}

# Prints the current workspace path, for debugging purposes
echo ${WORKSPACE}

# ---------------- --TEST BUILD ------------------
# The test log is called test_unit_log.txt
# Tests for all storage modes
if [ $test_build_enabled -eq 1 ]
then
  TEST_NAME="test_build"

  for ARCH in "X86" "RPI"
  do
    for ST_MODE in "RAM" "SQLITE" "FLASH"
    do
      echo ""
      echo "**** Testing ${TEST_NAME} for arch ${ARCH}, storage ${ST_MODE} ****"
      rm -rf build_test
      # Just build the simple app with the test's parameters
      cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=${ARCH} -DAPP=simple -DTEST=0 -DSCH_ST_MODE=${ST_MODE} -DSCH_STORAGE_TRIPLE_WR=1 && cmake --build build_test -j4
      status=$?
      [ $status -eq 0 ] || exit 1
    done
  done
  cd ${WORKSPACE}
fi


# ---------------- --TEST_UNIT ------------------
# The test log is called test_unit_log.txt
# Tests for all storage modes
if [ $test_unit_enabled -eq 1 ]
then
  TEST_NAME="test_unit"

  for ST_MODE in "RAM" "SQLITE" "FLASH"
  do
      echo ""
      echo "**** Testing ${TEST_NAME} for storage ${ST_MODE} ****"
      rm -rf build_test
      # build the unitest with the test's parameters
      cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=${ST_MODE} -DSCH_STORAGE_TRIPLE_WR=1 && cmake --build build_test -j4
      [ $? -eq 0 ] || exit $?

      # Runs the test, saving a log file
      cd build_test/test/${TEST_NAME}
      rm -f ${TEST_NAME}_${ST_MODE}_log.txt
      ./suchai-test #| cat >> ${TEST_NAME}_${ST_MODE}_log.txt
      [ $? -eq 0 ] || exit $?
      echo ""
      cd -
  done
  cd ${WORKSPACE}
fi


## ---------------- --TEST_INT_CMD ------------------
#
## The main test log is called test_int_cmd_log.txt
## That log is compares with test_int_cmd_log_base.txt
## The result of the comparison is in test_int_cmd_comparator_log.txt
#
if [ $test_cmd_enabled -eq 1 ]
then
  TEST_NAME="test_int_cmd"
  rm -rf build_test
  # build the cmdtest with the test's parameters
  cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=RAM && cmake --build build_test -j4
  [ $? -eq 0 ] || exit $?

  # Runs the test, saving a log file
  rm -f /test/${TEST_NAME}/${TEST_NAME}_log.txt
  ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
  [ $? -eq 0 ] || exit $?
  echo ""
  cd -
  cd ${WORKSPACE}
  #
  ## Makes result comparison
  cd test/${TEST_NAME}
  rm -f ${TEST_NAME}_comparator_log.txt
  python logs_comparator.py | cat >> ${TEST_NAME}_comparator_log.txt
  echo ""
  cd ${WORKSPACE}
  #
fi


## ---------------- --TEST_LOAD ------------------
#
## The test log is called test_int_load_log.txt
#
if [ $test_load_enabled -eq 1 ]
then
  TEST_NAME="test_int_load"

    echo ""
    echo "**** Testing ${TEST_NAME} ****"
    rm -rf build_test
    # build the test with the test's parameters
    cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=RAM  && cmake --build build_test -j4
    [ $? -eq 0 ] || exit $?

    ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
    [ $? -eq 0 ] || exit $?
    echo ""
    cd -

  cd ${WORKSPACE}
fi


## ---------------- --TEST_INT_BUG_DELAY ------------------
#
## The test log is called test_int_bug_delay_log.txt

if [ $test_bug_delay_enabled -eq 1 ]
then
  TEST_NAME="test_int_bug_delay"

    echo ""
    echo "**** Testing ${TEST_NAME} ****"
    rm -rf build_test
    # build the test with the test's parameters
    cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=RAM  && cmake --build build_test -j4
    [ $? -eq 0 ] || exit $?

    ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
    [ $? -eq 0 ] || exit $?
    echo ""
    cd -

  cd ${WORKSPACE}
fi

## ---------------- --TEST_INT_SGP4 ------------------
#
## The test log is called test_int_sgp4_log.txt

if [ $test_sgp4_enabled -eq 1 ]
then
  TEST_NAME="test_int_sgp4"

    echo ""
    echo "**** Testing ${TEST_NAME} ****"
    rm -rf build_test
    # build the test with the test's parameters
    cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=RAM  && cmake --build build_test -j4
    [ $? -eq 0 ] || exit $?

    ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
    [ $? -eq 0 ] || exit $?
    echo ""
    cd -

  cd ${WORKSPACE}
fi


## ---------------- --TEST_INT_TM_IO ------------------
#
## The test log is called test_int_tm_io_log.txt

if [ $test_tm_io_enabled -eq 1 ]
then
  TEST_NAME="test_int_tm_io"

    echo ""
    echo "**** Testing ${TEST_NAME} ****"
    rm -rf build_test
    # build the test with the test's parameters
    cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_ST_MODE=RAM -DSCH_COMM_NODE=3 && cmake --build build_test -j4
    [ $? -eq 0 ] || exit $?

    ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
    [ $? -eq 0 ] || exit $?
    echo ""
    cd -

  cd ${WORKSPACE}
fi

## ---------------- --TEST_INT_FILE-- ------------------
#
## The test log is called test_int_file_log.txt

if [ $test_file_enabled -eq 1 ]
then
  TEST_NAME="test_int_file"

    echo ""
    echo "**** Testing ${TEST_NAME} ****"
    rm -rf build_test
    # build the test with the test's parameters
    cmake -B build_test -G Ninja -DSCH_OS=LINUX -DSCH_ARCH=X86 -DAPP=${TEST_NAME} -DTEST=1 -DSCH_COMM_NODE=3  && cmake --build build_test -j4
    [ $? -eq 0 ] || exit $?

    ./build_test/test/${TEST_NAME}/suchai-test > test/${TEST_NAME}/${TEST_NAME}_log.txt
    [ $? -eq 0 ] || exit $?
    echo ""
    cd -

  cd ${WORKSPACE}
fi


#TODO: DELETE THIS EXIT AFTER FIXING THE REST OF THE TESTS!
exit 0
#TODO: REMEMBER TO DELETE THIS EXIT!