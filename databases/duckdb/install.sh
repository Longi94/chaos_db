#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

#VERSION=v0.1.1
VERSION=638c98d6e1efd2f5cabecfc61791fb1505c76edd
DIR=$(realpath $1)

cd ${DIR}
git clone https://github.com/cwida/duckdb
cd duckdb
git checkout ${VERSION} -b ${VERSION}
make
