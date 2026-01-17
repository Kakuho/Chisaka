#!/bin/bash

TEST=$1
COUNT=0

if [ $# -eq 0 ]; then
    echo "No arguments supplied"
    exit 0
fi

./make_test.sh ${TEST}
./run_test.sh ${TEST}
RET=$?
echo "RET: ${RET}"
if [ $RET -eq 124 ]; then
  echo "Test '${TEST}' hanged"
  exit 124
else
  TEST_RET=$((($RET & ~1) >> 1))
  if [ $TEST_RET -eq 0 ]; then
    echo "Test '${TEST}' success"
    exit 0
  else
    echo "Test '${TEST}' failed"
    exit 1
  fi
fi
