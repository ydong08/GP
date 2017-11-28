#!/bin/bash

if [ ! -d build ];then
	mkdir build
fi

cd build

if [ $# = 0 ];then
	cmake ..
else
	cmake -D__DDCRYPTO__=ON ..
fi

make
