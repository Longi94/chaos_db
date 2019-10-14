#!/bin/bash

DIR=$(realpath $1)
MAIN_VERSION=Apr2019-SP1
VERSION=11.33.11
MONETDB_DIR=MonetDB-${VERSION}

if [[ ! -f ${DIR}/${MONETDB_DIR}.tar.bz2 ]]; then
    wget -O ${DIR}/${MONETDB_DIR}.tar.bz2 https://www.monetdb.org/downloads/sources/${MAIN_VERSION}/${MONETDB_DIR}.tar.bz2
fi

tar xvf ${DIR}/${MONETDB_DIR}.tar.bz2 -C ${DIR}/

mkdir ${DIR}/bin
cd ${MONETDB_DIR}
./configure --prefix ${DIR}/bin/ --disable-strict --disable-assert --disable-debug --enable-optimize
make
make install

rm -rf ${DIR}/${MONETDB_DIR}

echo "user=monetdb\npassword=monetdb\n" > ~/.monetdb
