#!/usr/bin/bash
set -e
#first, we will build Sail
pushd .
cd external/sail
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX="../install" -DSAIL_EXCEPT_CODECS="" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++20" -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-rpath=\$ORIGIN:\$ORIGIN/../.." ..
make -j install
cd ../install/lib/sail
#find codecs -name '*.so' -exec sh -c 'mv "{}" codecs/lib$(basename "{}")' \;
popd
make -j kernel_filter_2d
echo "build complete."
