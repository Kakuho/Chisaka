#! /bin/bash

g++ -O0 -g -std=c++23 \
    -I./../../../../src  -I./../../../../ext \
    -DUSERMODE_TESTING \
    -o main main.cpp page_fuzzer.cpp ./memory_map_mock.cpp ./../../../../src/drivers/serial/kostream.cpp

./main
