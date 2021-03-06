#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

DIR=$(realpath $1)
AMALG_VER=3290000
ZIP_SOURCE=sqlite-amalgamation-${AMALG_VER}

if [[ ! -f ${DIR}/${ZIP_SOURCE}.zip ]]; then
    wget -O ${DIR}/${ZIP_SOURCE}.zip https://www.sqlite.org/2019/${ZIP_SOURCE}.zip
fi

unzip ${DIR}/${ZIP_SOURCE}.zip -d ${DIR}/
mkdir ${DIR}/bin || true
gcc ${DIR}/${ZIP_SOURCE}/shell.c ${DIR}/${ZIP_SOURCE}/sqlite3.c -lpthread -ldl -o ${DIR}/bin/sqlite3

rm -rf ${DIR}/${ZIP_SOURCE}
