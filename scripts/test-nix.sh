#!/bin/bash

(cd "cross-build/build/$1/bin" && exec ./lightship_tests) || exit 1;

exit 0
