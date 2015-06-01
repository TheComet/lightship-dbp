#!/bin/bash

(cd "cross-build/build/test-linux/bin" && exec ./lightship_tests) || exit 1;

exit 0
