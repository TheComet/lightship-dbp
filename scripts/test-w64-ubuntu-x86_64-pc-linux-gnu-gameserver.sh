#!/bin/bash

VERSION=$(cat lightship_version)
CROOT=x86_64-w64-mingw32
TEST_DIR="cross-build/build/test-$CROOT/bin"

./scripts/cross-compile.py \
    --set-version $VERSION \
    --output-name "test-$CROOT" \
    --compiler-root /usr/bin/$CROOT \
    --cmake "BUILD_TESTS=ON" \
    --make "make -j4" || exit 1;

(cd $TEST_DIR && exec ./lightship_tests) || exit 1;
    

echo "================================================"
echo "Done!"
echo "================================================"

exit 0
