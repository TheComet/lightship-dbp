#!/bin/bash

VERSION=$(cat lightship_version)

./scripts/cross-compile.py \
    --target Windows \
    --triplet x86_64-pc-mingw32 \
    --c-compiler /usr/bin/x86_64-pc-mingw32-gcc \
    --rc-compiler /usr/bin/x86_64-pc-mingw32-windres \
    --compiler-root /usr/bin/x86_64-pc-mingw32 \
    --set-version $VERSION \
    --make "make -j5" \
    --install "make install"

./scripts/cross-compile.py \
    --target Windows \
    --triplet i686-pc-mingw32 \
    --c-compiler /usr/bin/i686-pc-mingw32-gcc \
    --rc-compiler /usr/bin/i686-pc-mingw32-windres \
    --compiler-root /usr/bin/i686-pc-mingw32 \
    --set-version $VERSION \
    --make "make -j5" \
    --install "make install" \
    --cmake "ENABLE_WINDOWS_EX=OFF"

./scripts/cross-compile.py \
    --set-version $VERSION \
    --triplet x86_64-pc-linux-gnu \
    --make "make -j5" \
    --install "make install"

echo "================================================"
echo "Done!"
echo "================================================"

