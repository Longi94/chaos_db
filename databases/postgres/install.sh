#!/bin/bash

DIR=$(realpath $1)
VERSION=11.5
POSTGRES_DIR=postgresql-${VERSION}

if [[ ! -f ${DIR}/${POSTGRES_DIR}.tar.bz2 ]]; then
    wget -O ${DIR}/${POSTGRES_DIR}.tar.bz2 https://ftp.postgresql.org/pub/source/v${VERSION}/${POSTGRES_DIR}.tar.bz2
fi

tar xvf ${DIR}/${POSTGRES_DIR}.tar.bz2 -C ${DIR}/

mkdir ${DIR}/bin
cd ${POSTGRES_DIR}
./configure --prefix ${DIR}/bin/ --disable-debug --disable-cassert CFLAGS="-O3"
make
make install

rm -rf ${DIR}/${POSTGRES_DIR}
