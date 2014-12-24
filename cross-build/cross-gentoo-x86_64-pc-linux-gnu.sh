#!/bin/sh

# clean build and dist tree from last build
./clean.sh

# build everything
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCROSS_COMPILE_i486-pc-mingw32=ON -DCROSS_COMPILE_x86_64-pc-mingw32=ON -DCMAKE_INSTALL_PREFIX=$(pwd)/../dist/ ../.. || exit 1;
make -j5 || exit 1;
make install || exit 1;
cd ..

# correct output directory for host build
mkdir dist/x86_64-pc-linux-gnu || exit 1;
mv dist/bin dist/x86_64-pc-linux-gnu || exit 1;

# generate run scripts for each platform
echo -e "@ECHO OFF\nlightship.exe\nPAUSE" > dist/x86_64-pc-mingw32/bin/run.bat || exit 1;
echo -e "@ECHO OFF\nlightship.exe\nPAUSE" > dist/i486-pc-mingw32/bin/run.bat || exit 1;
echo -e "export LD_LIBRARY_PATH=.\n./lightship" > dist/x86_64-pc-linux-gnu/bin/run.sh || exit 1;

# compress
7za a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on dist/archive/lightship-x86_64-pc-mingw32.7z dist/x86_64-pc-mingw32 || exit 1;
7za a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on dist/archive/lightship-i486-pc-mingw32.7z dist/i486-pc-mingw32 || exit 1;
tar --xz -cf dist/archive/lightship-x86_64-pc-linux-gnu.tar.xz dist/x86_64-pc-linux-gnu || exit 1;

echo "Done!"

