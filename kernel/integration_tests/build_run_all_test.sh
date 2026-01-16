#!/bin/bash

COUNT=0
SUCCESS_COUNT=0
FAILED_COUNT=0

TESTS=("test2" "test1")
FAILED_TESTS=()
SUCCESS_TESTS=()

for i in "${TESTS[@]}"
do
  COUNT=$((COUNT+1))
  ./make_test.sh ${i}
  ./run_test.sh ${i}
  RET=$((($? & ~1) >> 1))
  if [ $RET -eq 0 ]; then
    SUCCESS_COUNT=$((${SUCCESS_COUNT} + 1))
    SUCCESS_TESTS=("${SUCCESS_TESTS[@]}" "${i}")
  else
    FAILED_COUNT=$((${FAILED_COUNT} + 1))
    FAILED_TESTS=("${FAILED_TESTS[@]}" "${i}")
  fi
done

echo "${SUCCESS_COUNT} / ${COUNT} Tests Succeded"
echo "${SUCCESS_COUNT} / ${COUNT} Tests Failed"

echo "Failed Tests:"

for i in "${FAILED_TESTS[@]}"
do
  echo "  ${i}"
done
