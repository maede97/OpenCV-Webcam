#!/bin/bash

mkdir -p build
set -e
cd build
cmake ..
make
echo "Success. Run \"./run.sh [camera]\" where [camera] is optional and is used to choose"