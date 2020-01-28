#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)
pushd ${DIR}

pushd examples/simple-benchmark
make main
popd

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22
do
    ./examples/simple-benchmark/build/benchmark tpc-h.duckdb ${i}
done
popd
