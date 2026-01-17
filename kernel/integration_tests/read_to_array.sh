#!/bin/bash

array=()

readarray -t array < ./INTEGRATION_LIST.txt

for i in "${array[@]}"
do
  echo ${i}
done
