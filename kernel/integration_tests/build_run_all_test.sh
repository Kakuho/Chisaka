#!/bin/bash

COUNT=0
TESTS=("test1" "test2")

for i in "${TESTS[@]}"
do
  ./make_test.sh ${i}
  ./run_test.sh ${i}
  if [[ $? -eq 0 ]]; then
    COUNT=$((COUNT+1))
  fi
done
