#! /bin/bash

g++ -g -std=c++23 \
    -I./../../../../src  -I./../../../../ext \
    -o main main.cpp page_fuzzer.cpp ./memory_map_mock.cpp

./main
