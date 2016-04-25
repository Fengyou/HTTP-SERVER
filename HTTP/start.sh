#! /bin/bash
ROOT_PATH=$(pwd)

LIB_PATH=${ROOT_PATH}/lib/lib
export LD_LIBRARY_PATH=${LIB_PATH}
env | grep LD_LIBRARY_PATH
./httpd 8080
