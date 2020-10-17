#!/bin/bash

# wget 
wget https://github.com/biomotion/eos-2020/releases/download/v1.0/arm-linux-toolchain-bin.4.0.2.tar.gz

docker build --rm -t eos-2020:latest .

# rm
rm arm-linux-toolchain-bin.4.0.2.tar.gz
