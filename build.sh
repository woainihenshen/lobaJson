#! /bin/sh
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j `cat /proc/cpuinfo | grep processor| wc -l`