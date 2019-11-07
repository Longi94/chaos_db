#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

VERSION=20e0f9c6b99daff0ab36e942310ffefc3d93751e
DIR=$(realpath $1)

pushd ${DIR}
git clone https://github.com/Longi94/AHEAD.git
pushd AHEAD
git submodule update --init --recursive
git checkout ${VERSION} -b ${VERSION}

./bootstrap.sh

pushd build/Release
cmake --build .

popd
popd
popd
