#!/bin/bash

VERSION=$(cat lightship_version)

./scripts/cross-compile.py \
    --target Windows \
    --compiler-root /usr/bin/x86_64-pc-mingw32 \
    --set-version $VERSION \
    --make "make -j7" \
    --install "make install" || exit 1;

./scripts/cross-compile.py \
    --target Windows \
    --compiler-root /usr/bin/i686-pc-mingw32 \
    --set-version $VERSION \
    --make "make -j7" \
    --install "make install" \
    --cmake "ENABLE_WINDOWS_EX=OFF" || exit 1;

./scripts/cross-compile.py \
    --set-version $VERSION \
    --compiler-root /usr/bin/x86_64-pc-linux-gnu \
    --make "make -j7" \
    --install "make install" || exit 1;

echo "================================================"
echo "Done!"
echo "================================================"

