#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)
MAIN_VERSION=Apr2019-SP1
VERSION=11.33.11
MONETDB_DIR=MonetDB-${VERSION}

cp ./.monetdb ~/.monetdb

if [[ ! -f ${DIR}/${MONETDB_DIR}.tar.bz2 ]]; then
    wget -O ${DIR}/${MONETDB_DIR}.tar.bz2 https://www.monetdb.org/downloads/sources/${MAIN_VERSION}/${MONETDB_DIR}.tar.bz2
fi

tar xvf ${DIR}/${MONETDB_DIR}.tar.bz2 -C ${DIR}/

mkdir -p ${DIR}/build || true
cd ${DIR}/${MONETDB_DIR}
./configure --prefix ${DIR}/build/ --disable-strict --disable-assert --disable-debug --enable-optimize
make
make install

rm -rf ${DIR}/${MONETDB_DIR}
