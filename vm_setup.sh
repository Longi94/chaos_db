#!/usr/bin/env bash

apt update
apt install -y cmake build-essential gcc gdb rsync zip python3-pip
pip3 install pandas

# SQLite3
dpkg --add-architecture i386
apt update
apt install -y libc6:i386 libncurses5:i386 libstdc++6:i386 zlib1g:i386
