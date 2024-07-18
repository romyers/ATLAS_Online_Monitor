#!/bin/bash

# sudo setcap cap_net_raw,cap_net_admin=eip DAQManager
sudo setcap cap_net_raw=eip build/DAQManager