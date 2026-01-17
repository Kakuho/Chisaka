#!/bin/bash

# still needs to add timeout for kernels that hang

TESTS=("test2" "test1" "hanging")
COUNT=0

SUCCESS_TESTS=()
SUCCESS_COUNT=0

FAILED_TESTS=()
FAILED_COUNT=0

HANGED_TESTS=()
HANGED_COUNT=0

for i in "${TESTS[@]}"
do
  COUNT=$((COUNT+1))
  ./make_test.sh ${i}
  ./run_test.sh ${i}
  RET=$?
  if [ $RET -eq 124 ]; then
    HANGED_COUNT=$((${HANGED_COUNT} + 1))
    HANGED_TESTS=("${HANGED_TESTS[@]}" "${i}")
  else
    TEST_RET=$((($RET & ~1) >> 1))
    if [ $TEST_RET -eq 0 ]; then
      SUCCESS_COUNT=$((${SUCCESS_COUNT} + 1))
      SUCCESS_TESTS=("${SUCCESS_TESTS[@]}" "${i}")
    else
      FAILED_COUNT=$((${FAILED_COUNT} + 1))
      FAILED_TESTS=("${FAILED_TESTS[@]}" "${i}")
    fi
  fi
done

# printing output to the screen

echo "${SUCCESS_COUNT} / ${COUNT} Tests Succeded"
echo "${HANGED_COUNT} / ${COUNT} Tests Hanged"
echo "${FAILED_COUNT} / ${COUNT} Tests Failed"

echo "Failed Tests:"

for i in "${FAILED_TESTS[@]}"
do
  echo "  ${i}"
done

echo "Hanged Tests:"

for i in "${HANGED_TESTS[@]}"
do
  echo "  ${i}"
done
