#!/bin/bash

mkdir build

cd build

cmake -DBUILD_EXE=ON ..
make
sudo setcap cap_net_raw=eip ecap

cd ..