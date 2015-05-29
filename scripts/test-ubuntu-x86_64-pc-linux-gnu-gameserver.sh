#!/bin/bash

VERSION=$(cat lightship_version)
CHOST=x86_64-pc-linux-gnu
HOST_TEST_DIR="cross-build/build/test-$CHOST/bin"

./scripts/cross-compile.py \
    --set-version $VERSION \
    --output-name "test-$CHOST" \
    --compiler-root /usr/bin/$CHOST \
    --cmake "BUILD_TESTS=ON" \
    --make "make -j4" || exit 1;

(cd $HOST_TEST_DIR && exec ./lightship_tests) || exit 1;
    

echo "================================================"
echo "Done!"
echo "================================================"

exit 0
