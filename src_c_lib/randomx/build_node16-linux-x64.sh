#!/bin/bash
set -e

rm -rf build_node16-linux-x64 2>/dev/null || echo "skip rm build_node16-linux-x64"
cd repo

rm -rf build 2>/dev/null || echo "skip rm build"
mkdir build
cd build
cmake $* ..
make -j$(nproc)
cd ..
mv build ../build_node16-linux-x64
