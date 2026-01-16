#/bin/bash

PROJECT_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring"
KERNEL_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring/kernel"
INTEGRATION_ROOT="${KERNEL_ROOT}/integration_tests"
SRC_ROOT="${KERNEL_ROOT}/src"

TEST_NAME=$1
TEST_DIR="${INTEGRATION_ROOT}/${TEST_NAME}"

IMAGE_NAME=${TEST_NAME}.iso

#qemu-system-x86_64  -cpu IvyBridge -M q35 -m 8G -no-reboot \
#                    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
#									  -cdrom ${TEST_DIR}/dist/${IMAGE_NAME} \
#									  -boot d 

qemu-system-x86_64  -cpu IvyBridge -M q35 -m 8G -no-reboot \
                    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
									  -cdrom ${TEST_DIR}/dist/${IMAGE_NAME} \
                    -display none \
                    -boot d

#echo $((($? & ~1) >> 1))
