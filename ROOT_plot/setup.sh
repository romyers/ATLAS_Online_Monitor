#!/bin/bash

mkdir build

cd build
cmake ..
make DAQManager
sudo setcap cap_net_raw,cap_net_admin=eip DAQManager

