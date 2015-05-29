#!/bin/bash

VERSION=$(cat lightship_version)
CROOT=x86_64-w64-mingw32
TEST_DIR="cross-build/build/test-$CROOT/bin"

./scripts/cross-compile.py \
    --platform Windows \
    --set-version $VERSION \
    --output-name "test-$CROOT" \
    --compiler-root /usr/bin/$CROOT \
    --cmake "ENABLE_WINDOWS_EX=OFF" \
    --cmake "BUILD_TESTS=ON" \
    --make "make -j4" || exit 1;

(cd $TEST_DIR && wineconsole lightship_tests.exe) || exit 1;
    

echo "================================================"
echo "Done!"
echo "================================================"

exit 0
