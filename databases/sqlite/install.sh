#!/bin/bash

DIR=$(realpath $1)
AMALG_VER=3290000
ZIP_SOURCE=sqlite-amalgamation-${AMALG_VER}

if [[ ! -f ${DIR}/${ZIP_SOURCE}.zip ]]; then
    wget -O ${DIR}/${ZIP_SOURCE}.zip https://www.sqlite.org/2019/${ZIP_SOURCE}.zip
fi

unzip ${DIR}/${ZIP_SOURCE}.zip -d ${DIR}/
mkdir ${DIR}/bin
gcc ${DIR}/${ZIP_SOURCE}/shell.c ${DIR}/${ZIP_SOURCE}/sqlite3.c -lpthread -ldl -o bin/sqlite3

rm -rf ${DIR}/${ZIP_SOURCE}
