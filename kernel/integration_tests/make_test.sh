#/bin/bash

PROJECT_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring"
KERNEL_ROOT="${PROJECT_ROOT}/kernel"
INTEGRATION_ROOT="${KERNEL_ROOT}/integration_tests"
SRC_ROOT="${KERNEL_ROOT}/src"

TEST_NAME=$1
TEST_DIR="${INTEGRATION_ROOT}/test_builds/${TEST_NAME}"

ENTRY_POINT="${SRC_ROOT}/integration_tests/${TEST_NAME}.cpp"
IMAGE_NAME="TEST.iso"

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

# check to see if the test entry point exists

if [ -f ${ENTRY_POINT} ]; then
  echo "entry point ${ENTRY_POINT} exists"
else
  echo "entry point doesnt exist"
  exit 1
fi

# reset its test directory

if [  -d ${TEST_DIR} ]; then
  rm -r ${TEST_DIR}
fi

mkdir -p ${TEST_DIR}

# setting up compiler and linker

CPP_COMPILER=/usr/local/bin/g++
C_COMPILER=/usr/local/bin/gcc
LINKER=/usr/bin/ld

# Flags setting

CPP_FLAGS="-g -O0 -std=c++20 -pipe -Wall -Wextra -Wpointer-arith -Wcast-align -Wswitch-enum \
          -fno-rtti -fno-exceptions"
C_FLAGS=
PREPROCESSOR_FLAGS="-MMD -MP"

SHARED_FLAGS="-g -O0 -Wall -Wextra \
             -ffreestanding -nostdlib \
             -fno-stack-protector  -fno-stack-check -fno-lto \
             -fPIE -fPIC \
             -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone \
             -isystem include"

LD_FLAGS="-m elf_x86_64 -nostdlib -static --no-dynamic-linker \
        -z text -z max-page-size=0x1000
        -T ${KERNEL_ROOT}/scripts/linker.ld"

# get all the cpp and asm source files

SRC_FILES=""

cd ${SRC_ROOT}

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find -L * -type f -name '*.cpp' -not -path "integration_tests/*" -not -path "main.cpp" -print0) # cpp

for i in "${array[@]}"
do
   i=${SRC_ROOT}/${i}
   #echo "${i}"
   SRC_FILES+="${i} "
done

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find -L * -type f -name '*.S' -not -path "integration_tests/*" -not -path "main.cpp" -print0) # asm

for i in "${array[@]}"
do
   i=${SRC_ROOT}/${i}
   #echo "${i}"
   SRC_FILES+="${i} "
done

cd ${INTEGRATION_ROOT}

# compile the test entry point executable file

cd ${TEST_DIR}

${CPP_COMPILER} -g ${CPP_FLAGS} ${SHARED_FLAGS} ${PREPROCESSOR_FLAGS} -c ${ENTRY_POINT} ${SRC_FILES} -I${SRC_ROOT} -I${KERNEL_ROOT}/ext

cd ${INTEGRATION_ROOT}

# link the kernel build objects with the test objects

cd ${TEST_DIR}

OBJECT_FILES=$(find -L * -type f -name '*.o')
echo $OBJECT_FILES

$LINKER $OBJECT_FILES $LD_FLAGS -o kernel

cd ${INTEGRATION_ROOT}

# now we need to repackage the kernel as a iso

cd ${TEST_DIR}
echo $PWD

ISOROOT=./iso_root
if [ ! -d ${ISOROOT} ]; then
  mkdir ${ISOROOT}
else
  rm -r iso_root
  mkdir ${ISOROOT}
fi

echo ${ISOROOT}

mkdir -p ${ISOROOT}/boot
cp -v ${TEST_DIR}/kernel ${ISOROOT}/boot/

mkdir -p ${ISOROOT}/boot/limine
cp -v ${PROJECT_ROOT}/limine.cfg ${PROJECT_ROOT}/limine/limine-bios.sys ${PROJECT_ROOT}/limine/limine-bios-cd.bin ${PROJECT_ROOT}/limine/limine-uefi-cd.bin ${ISOROOT}/boot/limine/

mkdir -p ${ISOROOT}/EFI/BOOT
cp -v ${PROJECT_ROOT}/limine/BOOTX64.EFI ${ISOROOT}/EFI/BOOT/
cp -v ${PROJECT_ROOT}/limine/BOOTIA32.EFI ${ISOROOT}/EFI/BOOT/

xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  --efi-boot boot/limine/limine-uefi-cd.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  ${ISOROOT} -o ${IMAGE_NAME}

${PROJECT_ROOT}/limine/limine bios-install ${TEST_DIR}/${IMAGE_NAME}
rm -rf ${ISOROOT}
rm -rf ${TEST_DIR}/bin
mkdir ${TEST_DIR}/bin
mv ${TEST_DIR}/${IMAGE_NAME} ${TEST_DIR}/bin

exit 0
