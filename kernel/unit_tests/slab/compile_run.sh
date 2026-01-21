#/bin/bash

g++ -std=c++20 -o main ./list_descriptor/linkage.cpp\
      -I..  -I../../ext  -I../../src\
      ./../test_main.cpp 
