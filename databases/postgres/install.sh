#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)
VERSION=11.5
POSTGRES_DIR=postgresql-${VERSION}

if [[ ! -f ${DIR}/${POSTGRES_DIR}.tar.bz2 ]]; then
    wget -O ${DIR}/${POSTGRES_DIR}.tar.bz2 https://ftp.postgresql.org/pub/source/v${VERSION}/${POSTGRES_DIR}.tar.bz2
fi

tar xvf ${DIR}/${POSTGRES_DIR}.tar.bz2 -C ${DIR}/

mkdir ${DIR}/build 2>/dev/null || true
cd ${DIR}/${POSTGRES_DIR}
./configure --prefix ${DIR}/build/ --disable-debug --disable-cassert CFLAGS="-O3"
make
make install

rm -rf ${DIR}/${POSTGRES_DIR}
