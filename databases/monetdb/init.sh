#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

MONETDB_DIR=$(realpath $1)
DATA_DIR=$(realpath $2)
DB_NAME=$(basename ${DATA_DIR})

mkdir ${DATA_DIR}
nohup ${MONETDB_DIR}/build/bin/mserver5 --dbpath ${DATA_DIR} --daemon=yes &
PID=$!

until ${MONETDB_DIR}/build/bin/mclient -d ${DB_NAME} -s "SELECT 1"
do
  echo "Waiting for server..."
  sleep 1
done

${MONETDB_DIR}/build/bin/mclient -d ${DB_NAME} schema.sql
python3 generate_load_file.py
${MONETDB_DIR}/build/bin/mclient -d ${DB_NAME} load.sql.tmp
${MONETDB_DIR}/build/bin/mclient -d ${DB_NAME} constraints.sql
kill ${PID}
