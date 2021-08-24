#!/bin/sh

mkdir build
cd build
cmake -DNektar++_DIR=/home/liubin/nektar-v5.0.2/build -DNEKTAR_BUILD_DOCS=ON ..
make -j 4 install 



