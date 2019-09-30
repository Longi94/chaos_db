#!/bin/bash

TPCH_VERSION=2.18.0_rc2

unzip tpc-h-tool.zip
cp Makefile $TPCH_VERSION/dbgen/
cd $TPCH_VERSION/dbgen/ || exit
make
./dbgen -v -f -s 1

cd ../..
mkdir tables
mv $TPCH_VERSION/dbgen/*.tbl tables/
