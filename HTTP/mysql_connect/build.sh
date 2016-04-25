#! /bin/bash

LIB_PATH=/home/yuantian/mystudy/my_exercise/HTTP/lib/lib
export LD_LIBRARY_PATH=${LIB_PATH}
env | grep LD_LIBRARY_PATH
make clean;make
