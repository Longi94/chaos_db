#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

VERSION=cec5c8f7258b84aa37230ff2095ec4f55da3c56a
DIR=$(realpath $1)

cd ${DIR}
git clone https://github.com/Longi94/AHEAD.git
cd AHEAD
git checkout ${VERSION} -b ${VERSION}

./bootstrap.sh

cd build/Release
cmake --build .
