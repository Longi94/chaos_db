#!/usr/bin/env bash

apt update
apt install -y cmake build-essential gcc gdb rsync zip python3-pip libsqlite3-dev
pip3 install pandas jupyter matplotlib

mkdir /home/vagrant/.jupyter
cp /vagrant/jupyter/jupyter_notebook_config.py /home/vagrant/.jupyter/

# SQLite3
dpkg --add-architecture i386
apt update
apt install -y libc6:i386 libncurses5:i386 libstdc++6:i386 zlib1g:i386

# postgres
apt install -y libreadline-dev zlib1g-dev

# AHEAD
apt install -y libboost-all-dev libsparsehash-dev libffi-dev libncurses5-dev

# monetdb
apt install -y bison libssl-dev
