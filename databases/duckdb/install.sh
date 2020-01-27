#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)

cd ${DIR}
git clone https://github.com/Longi94/duckdb
cd duckdb
git checkout chaos
make
cd examples/wrapper
make main

