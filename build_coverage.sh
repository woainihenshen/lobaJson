#! /bin/sh
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_C_FLAGS=--coverage ..
make -j `cat /proc/cpuinfo | grep processor| wc -l`
ctest
pwd
coveralls -r .. --gcov-options '\-lp' -t PAfc6k2qXkZzTkTKfkFUQWkzxGHI7qMem -e tests -e build/CMakeFiles

#test1
#test2
#test3
#test4
#test5
#test6
