#!/bin/bash

VERSION=0.1.1

git clone https://github.com/cwida/duckdb
cd duckdb
git checkout v${VERSION} -b v${VERSION}
make
cd ..
