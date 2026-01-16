#/bin/bash

KERNEL_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring/kernel"
INTEGRATION_ROOT="${KERNEL_ROOT}/integration_tests"
SRC_ROOT="${KERNEL_ROOT}/src"

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

# Checking if the integration directory exists

if [ -d ${INTEGRATION_ROOT} ]; then
  echo "Integration Root exists"
else
  echo "Integration Root does not exist, please ensure it is set correctly"
  exit 1
fi

# Setting up the tools

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

# Getting C++ Source File

CPP_FILES=""

cd ${SRC_ROOT}

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find -L * -type f -name '*.cpp' -not -path "entry_points/*" -print0)

for i in "${array[@]}"
do
   i=${SRC_ROOT}/${i}
   #echo "${i}"
   CPP_FILES+="${i} "
done

echo "Printing CPP Files:"
echo ${CPP_FILES}

cd ${INTEGRATION_ROOT}

# Getting Assembly Source File

ASM_FILES=""

cd ${SRC_ROOT}

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find -L * -type f -name '*.S' -not -path "entry_points/*" -print0)

for i in "${array[@]}"
do
   i=${SRC_ROOT}/${i}
   #echo "${i}"
   ASM_FILES+="${i} "
done

echo "Printing Assembly Files:"
echo ${ASM_FILES}

cd ${INTEGRATION_ROOT}

# checking if the limine file is here

if [ -f ${SRC_ROOT}/firmware/limine/limine.h ]; then
  echo "Limine already exists"
else
  echo "downloading limine..."
  #curl -Lo https://github.com/limine-bootloader/limine/raw/trunk/limine.h
fi

# compile the object files

echo "building src object files..."
mkdir ${INTEGRATION_ROOT}/build_integ
echo "${INTEGRATION_ROOT}/build_integ"
cd ${INTEGRATION_ROOT}/build_integ
${CPP_COMPILER} ${CPP_FLAGS} ${SHARED_FLAGS} ${PREPROCESSOR_FLAGS} -c ${CPP_FILES} ${ASM_FILES} -I${SRC_ROOT} -I${KERNEL_ROOT}/ext

cd ${INTEGRATION_ROOT}

# linking the kernel w.r.t to the kernel script

echo "linking kernel..."

OBJECT_FILES=$(find -L * -type f -name '*.o')
echo $OBJECT_FILES

$LINKER $OBJECT_FILES $LD_FLAGS -o kernel
