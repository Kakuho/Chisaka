#/bin/bash

g++ -std=c++20 -o main ./get_val1_test.cpp ./get_val2_test.cpp ./get_val3_test.cpp \
      -I.. -I../.. \
      ./../test_main.cpp 
