#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

AHEAD_DIR=$(realpath $1)
DB_PATH=$(realpath $2)

mkdir -p ${DB_PATH}

pushd ${AHEAD_DIR}/database

${AHEAD_DIR}/database/dbgen -s 1 -T c -v -f
${AHEAD_DIR}/database/dbgen -s 1 -T d -v -f
${AHEAD_DIR}/database/dbgen -s 1 -T l -v -f
${AHEAD_DIR}/database/dbgen -s 1 -T p -v -f
${AHEAD_DIR}/database/dbgen -s 1 -T s -v -f

popd

pushd ${DB_PATH}

tar -vxaf ${AHEAD_DIR}/headers.tgz

mv database/headers/*.csv .
cp ${AHEAD_DIR}/database/*.tbl .

ln -s customer.tbl customerAN.tbl
ln -s date.tbl dateAN.tbl
ln -s lineorder.tbl lineorderAN.tbl
ln -s part.tbl partAN.tbl
ln -s supplier.tbl supplierAN.tbl

${AHEAD_DIR}/build/Release/ssbm-dbsize_scalar -d .

rm -rf *.tbl

popd