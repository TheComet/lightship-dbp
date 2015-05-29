#!/bin/bash

VERSION=$(cat lightship_version)

./scripts/cross-compile.py \
    --platform Windows \
    --set-version $VERSION \
    --compiler-root /usr/bin/x86_64-pc-mingw32 \
    --cmake "CMAKE_BUILD_TYPE=Release" \
    --make "make -j7" \
    --install "make install" \
    --compress 7z || exit 1;

./scripts/cross-compile.py \
    --platform Windows \
    --set-version $VERSION \
    --compiler-root /usr/bin/i686-pc-mingw32 \
    --cmake "CMAKE_BUILD_TYPE=Release" \
    --cmake "ENABLE_WINDOWS_EX=OFF" \
    --make "make -j7" \
    --install "make install" \
    --compress 7z || exit 1;

./scripts/cross-compile.py \
    --set-version $VERSION \
    --compiler-root /usr/bin/x86_64-pc-linux-gnu \
    --cmake "CMAKE_BUILD_TYPE=Release" \
    --make "make -j7" \
    --install "make install" \
    --compress xz || exit 1;

echo "================================================"
echo "Done!"
echo "================================================"

exit 0
