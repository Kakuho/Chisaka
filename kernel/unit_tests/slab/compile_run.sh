#/bin/bash

SRC_DIR="/home/ka/C++/projects/os/Chisaka/git/slab_tests/kernel/src"
CPPSRC="${SRC_DIR}/kheap/slab/list_descriptor.cpp"

g++ -std=c++20 -D USERMODE_TESTING -o tests ./list_descriptor/linkage.cpp\
      ${CPPSRC} \
      -I..  -I../../ext  -I../../src\
      ./../test_main.cpp 

./tests
