#!/bin/sh
cd $(dirname $0)
cmake -S src -B build
cd build
make
cd ..
rm hsh
cp build/hsh .
