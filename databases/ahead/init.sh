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

echo "Generating customers table..."
${AHEAD_DIR}/database/dbgen -s 1 -T c -f 1>/dev/null 2>/dev/null
echo "Generating date table..."
${AHEAD_DIR}/database/dbgen -s 1 -T d -f 1>/dev/null 2>/dev/null
echo "Generating lineitems table..."
${AHEAD_DIR}/database/dbgen -s 1 -T l -f 1>/dev/null 2>/dev/null
echo "Generating parts table..."
${AHEAD_DIR}/database/dbgen -s 1 -T p -f 1>/dev/null 2>/dev/null
echo "Generating suppliers table..."
${AHEAD_DIR}/database/dbgen -s 1 -T s -f 1>/dev/null 2>/dev/null

# For some reason, files can randomly have 101
chmod 644 *tbl

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

${AHEAD_DIR}/build/Release/ssbm-init -d .

rm *.tbl
rm -rf database

popd
