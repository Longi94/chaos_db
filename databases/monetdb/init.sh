#!/bin/bash

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

MONETDB_DIR=$(realpath $1)
DATA_DIR=$(realpath $2)

${MONETDB_DIR}/bin/bin/monetdbd create ${DATA_DIR}
${MONETDB_DIR}/bin/bin/monetdbd start ${DATA_DIR}
${MONETDB_DIR}/bin/bin/monetdb destroy tpch || true
${MONETDB_DIR}/bin/bin/monetdb create tpch
${MONETDB_DIR}/bin/bin/monetdb release tpch
${MONETDB_DIR}/bin/bin/mclient -d tpch schema.sql
python generate_load_file.py
${MONETDB_DIR}/bin/bin/mclient -d tpch load.sql.tmp
${MONETDB_DIR}/bin/bin/mclient -d tpch constraints.sql
${MONETDB_DIR}/bin/bin/monetdbd stop ${DATA_DIR}
