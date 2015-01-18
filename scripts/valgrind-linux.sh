#!/bin/sh
cd build/bin
valgrind --num-callers=50 \
	--leak-resolution=high \
	--leak-check=full \
	--track-origins=yes \
	--show-reachable=yes \
	--show-possibly-lost=yes  \
	--time-stamp=yes \
	--suppressions=../../scripts/valgrind-suppressions.supp \
	./lightship
cd .. && cd ..
