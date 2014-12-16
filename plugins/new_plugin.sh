#!/bin/sh
if [ -z ${1+x} ]; then echo "Please set a plugin name with \"./new_plugin my_plugin_name\". MUST be all lower-case and have no dashes."; exit 1; fi
cmake -DNAME="$1" plugin_template/
rm -r CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
