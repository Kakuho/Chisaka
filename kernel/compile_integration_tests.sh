#/bin/bash

SRC_ROOT="/home/ka/C++/projects/os/Chisaka/git/refactoring/kernel/src"

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
        -T scripts/linker.ld"

# Getting C++ Source File

CPP_FILES=""

cd src

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

cd ..

# Getting Assembly Source File

ASM_FILES=""

cd src

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

cd .. 

# checking if the limine file is here

if [ -f src/firmware/limine.h ]; then
  echo "Limine already exists"
else
  echo "downloading limine..."
  #curl -Lo https://github.com/limine-bootloader/limine/raw/trunk/limine.h
fi

# compile the object files

echo "building src object files..."
mkdir build_integ
cd ./build_integ
$CPP_COMPILER $CPP_FLAGS $SHARED_FLAGS $PREPROCESSOR_FLAGS -c ${CPP_FILES} ${ASM_FILES} -I../src -I../ext

cd ..

# start integration testing - we will require a test runner for this ...
# this is the only part that needs to change w.r.t integration testing

ENTRY_POINT="${SRC_ROOT}/entry_points/main.cpp"

echo "building entrypoint object file..."
cd ./build_integ
$CPP_COMPILER $CPP_FLAGS $SHARED_FLAGS $PREPROCESSOR_FLAGS -c $ENTRY_POINT -I../src -I../ext

cd ..

# linking the kernel w.r.t to the kernel script

echo "linking kernel..."

OBJECT_FILES=$(find -L * -type f -name '*.o')
echo $OBJECT_FILES

$LINKER $OBJECT_FILES $LD_FLAGS -o kernel
