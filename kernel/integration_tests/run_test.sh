#/bin/bash

PROJECT_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring"
KERNEL_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring/kernel"
INTEGRATION_ROOT="${KERNEL_ROOT}/integration_tests"
SRC_ROOT="${KERNEL_ROOT}/src"

if [ $# -eq 0 ]; then
    echo "No arguments supplied"
    exit -1
fi

TEST_NAME=$1
TEST_DIR="${INTEGRATION_ROOT}/test_builds/${TEST_NAME}"

if [ ! -d ${TEST_DIR} ]; then
  echo "Test Directory for test '${TEST_NAME}' does not exist"
  exit -1
else
  echo "Test Directory for test '${TEST_NAME}' exists"
fi

# Setting up the image file

IMAGE_FILE=${TEST_DIR}/bin/TEST.iso

if [ ! -f ${IMAGE_FILE} ]; then
  echo "The kernel image for test '${TEST_NAME}' does not exist"
  exit -1
else
  echo "Kernel Image for test '${TEST_NAME}' exist"
fi

# set up logging for the test

LOG_DIR=logs/${TEST_NAME}

if [ ! -d ${LOG_DIR} ]; then
  echo "Log dir does not exist"
  echo "Creating Log directory..."
  mkdir -p ${LOG_DIR}
fi

LOG_FILE=${LOG_DIR}/LOGS

if [ -f ${LOG_FILE} ]; then
  rm ${LOG_FILE}
fi

touch ${LOG_FILE}

# now we can invoke qemu

#timeout --foreground 10s qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G -no-reboot \
#                    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
#									  -cdrom ${IMAGE_FILE} \
#                    -serial file:${LOG_FILE} \
#                    -nographic \
#                    -boot d

timeout --foreground 10s qemu-system-x86_64 -cpu IvyBridge -M q35 -m 8G -no-reboot \
                    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
									  -cdrom ${IMAGE_FILE} \
                    -serial file:${LOG_FILE} \
                    -boot d

# print the return value of the test

RET=$?

if [ $RET -eq 124 ]; then
  echo "Test '${TEST_NAME}' hanged"
else
  TEST_RET=$((($RET & ~1) >> 1))
  if [ $TEST_RET -eq 0 ]; then
    echo "Test '${TEST_NAME}' success"
  else
    echo "Test '${TEST_NAME}' failed"
  fi
fi

exit ${RET}
