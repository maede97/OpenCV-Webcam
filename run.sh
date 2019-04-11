#!/bin/bash

./build/main | ffmpeg -i pipe:0 -pix_fmt yuv420p -f v4l2 /dev/video23