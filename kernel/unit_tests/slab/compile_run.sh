#/bin/bash

SRC_DIR="/home/ka/C++/projects/os/Chisaka/git/slab_tests/kernel/src"
CPPSRC="${SRC_DIR}/kheap/slab/list_descriptor.cpp"
TEST_SRC="./list_descriptor/linkage.cpp 
          ./list_descriptor/init.cpp
          ./list_descriptor/allocation.cpp
          ./list_descriptor/memory_patterns.cpp"
TEST_SRC="
  ./list_descriptor/memory_patterns.cpp
  ./list_descriptor/memory_patterns_page.cpp
"

g++ -std=c++20 -D USERMODE_TESTING -g -o tests \
      ${TEST_SRC} ${CPPSRC} \
      -I..  -I../../ext  -I../../src\
      ./../test_main.cpp 

./tests
