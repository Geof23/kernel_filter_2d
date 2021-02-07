#!/usr/bin/bash

#first, we will build Sail

cd external/sail
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="../install" -DSAIL_EXCEPT_CODECS="" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++20" ..
make -j


