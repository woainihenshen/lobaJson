#! /bin/sh
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_C_FLAGS=--coverage ..
make -j `cat /proc/cpuinfo | grep processor| wc -l`
cd tests
./lobaJsonTest
cd  ../..
coveralls