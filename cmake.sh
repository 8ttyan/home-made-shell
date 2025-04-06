#!/bin/sh
cd $(dirname $0)
cmake -S src -B build
cd build
make
cd ..
rm hms 2>/dev/null
cp build/hms .
