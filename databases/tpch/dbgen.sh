#!/bin/bash

set -e

git clone https://github.com/gregrahn/tpch-kit || true
cd tpch-kit/dbgen
make
./dbgen -v -f -s 1
./dbgen -v -U 1 -s 50

cd ../..
mkdir tables 2>/dev/null || true
mv tpch-kit/dbgen/*.tbl tables/

mkdir updates 2>/dev/null || true
mv tpch-kit/dbgen/*.tbl.u* updates/
