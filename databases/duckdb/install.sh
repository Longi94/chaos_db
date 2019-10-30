#!/bin/bash

set -e

VERSION=0.1.1

git clone https://github.com/cwida/duckdb
git checkout v${VERSION} -b v${VERSION}
make
