#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)

pushd ${DIR}
git clone https://github.com/Longi94/AHEAD.git
pushd AHEAD
git submodule update --init --recursive

./bootstrap.sh

pushd build/Release
cmake --build .

popd
popd
popd
