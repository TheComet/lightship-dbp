#!/bin/bash

# clean build and dist tree from last build
./clean.sh

WIN64=lightship-$1-$2-x86_64-pc-mingw32
LINUX64=lightship-$1-$2-x86_64-pc-linux-gnu

# build everything
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_HOST="twilight" \
	-DMAKE_ARGS=-j5 \
	-DCROSS_COMPILE_x86_64-pc-mingw32=ON \
	-Dx86_64-pc-mingw32_INSTALL_PREFIX=$(pwd)/../dist/$WIN64 \
	-DCMAKE_INSTALL_PREFIX=$(pwd)/../dist/$LINUX64 \
	../.. || exit 1;
make -j5 || exit 1;
make install || exit 1;
cd ..

# generate run scripts for each platform
echo -e "@ECHO OFF\ncd bin\nlightship.exe\nPAUSE\ncd .." > dist/$WIN64/run.bat || exit 1;
echo -e "export LD_LIBRARY_PATH=\$(pwd)/lib\ncd bin\n./lightship\ncd .." > dist/$LINUX64/run.sh || exit 1;

# compress
7za a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on dist/archive/$WIN64\.7z ./dist/$WIN64/ || exit 1;
tar --xz -cvf dist/archive/$LINUX64\.tar.xz -C dist/ $LINUX64/ || exit 1;

echo "================================================"
echo "Done!"
echo "================================================"

