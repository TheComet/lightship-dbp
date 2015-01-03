#!/bin/sh

# clean build and dist tree from last build
./clean.sh

# build everything
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
	-DBUILD_HOST="game.pixelcloud.ch" \
	-DCROSS_COMPILE_x86_64-pc-mingw32=ON \
	-Dx86_64-pc-mingw32_INSTALL_PREFIX=$(pwd)/../dist/x86_64-pc-mingw32 \
	-DCMAKE_INSTALL_PREFIX=$(pwd)/../dist/x86_64-pc-linux-gnu \
	../.. || exit 1;
make -j9 || exit 1;
make install || exit 1;
cd ..

# generate run scripts for each platform
echo -e "@ECHO OFF\ncd bin\nlightship.exe\nPAUSE\ncd .." > dist/x86_64-pc-mingw32/run.bat || exit 1;
echo -e "export LD_LIBRARY_PATH=\$(pwd)/lib\ncd bin\n./lightship\ncd .." > dist/x86_64-pc-linux-gnu/run.sh || exit 1;

# compress
LINUX64=lightship-$1-$2-x86_64-pc-linux-gnu.tar.xz
WIN64=lightship-$1-$2-x86_64-pc-mingw32.7z
7za a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on dist/archive/$WIN64 dist/x86_64-pc-mingw32 || exit 1;
tar --xz -cf dist/archive/$LINUX64 dist/x86_64-pc-linux-gnu || exit 1;

# move to distribution folder
echo "moving archives to /www/game/lightship/"
mv dist/archive/$WIN64 /www/game/lightship/
mv dist/archive/$LINUX64 /www/game/lightship/

echo "================================================"
echo "Done!"
echo "================================================"

