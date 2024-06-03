#!/bin/bash

cd build
sudo setcap cap_net_raw,cap_net_admin=eip DAQManager
cd ..