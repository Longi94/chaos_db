#!/bin/bash

set -e

git clone https://github.com/gregrahn/tpch-kit
cd tpch-kit/dbgen
make
./dbgen -v -f -s 1

cd ../..
mkdir tables
mv tpch-kit/dbgen/*.tbl tables/
