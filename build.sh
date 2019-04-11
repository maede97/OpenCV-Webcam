#!/bin/bash

mkdir -p build
set -e
cd build
cmake ..
make
echo "Success. Run ./run.sh"