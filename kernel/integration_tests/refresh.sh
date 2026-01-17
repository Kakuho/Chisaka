#!/bin/bash

# looks into the integration file directory and adds every file to integ_tests.txt

# first set up the integration list

PROJECT_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring"
KERNEL_ROOT="${PROJECT_ROOT}/kernel"
INTEGRATION_ROOT="${KERNEL_ROOT}/integration_tests"
SRC_ROOT="${KERNEL_ROOT}/src"

# basic checks to see if the directories exists

if [ -d ${KERNEL_ROOT} ]; then
  echo "Kernel Directory exists"
else
  echo "Kernel Directory does not exist, please ensure it is set correctly"
  exit 1
fi

if [ -d ${SRC_ROOT} ]; then
  echo "Source Directory exists"
else
  echo "Source Directory does not exist, please ensure it is set correctly"
  exit 1
fi

if [ -d ${INTEGRATION_ROOT} ]; then
  echo "Integration Root exists"
else
  echo "Integration Root does not exist, please ensure it is set correctly"
  exit 1
fi

# now we start the logic proper

INTEGRATION_LIST="INTEGRATION_LIST.txt"
INTEGRATION_SRCDIR="${SRC_ROOT}/integration_tests"

if [ -f ${INTEGRATION_LIST} ]; then
  rm ${INTEGRATION_LIST}
fi

touch ${INTEGRATION_LIST}

# now start the parsing

cd ${INTEGRATION_SRCDIR}

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find -L * -type f -name '*.cpp' -print0) 

# finally remove the cpp extension

cd ${INTEGRATION_ROOT}

for i in "${array[@]}"
do
   TEST_NAME="${i:0:-4}"
   echo ${TEST_NAME} >> ${INTEGRATION_LIST}
done
