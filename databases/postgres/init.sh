#!/bin/bash

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

POSTGRES_DIR=$(realpath $1)
PGDATA=$(realpath $2)

${POSTGRES_DIR}/bin/initdb -D ${PGDATA}
${POSTGRES_DIR}/bin/pg_ctl -D ${PGDATA} -l ${POSTGRES_DIR}/postgres.log start
${POSTGRES_DIR}/bin/dropdb tpch || true
${POSTGRES_DIR}/bin/createdb tpch
${POSTGRES_DIR}/bin/psql tpch -f schema.sql
${POSTGRES_DIR}/bin/psql tpch -f constraints.sql
${POSTGRES_DIR}/bin/psql tpch -f load.sql
${POSTGRES_DIR}/bin/pg_ctl -D ${PGDATA} -l ${POSTGRES_DIR}/postgres.log stop
