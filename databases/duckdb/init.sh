#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

DUCKDB_DIR=$(realpath $1)
DB_PATH=$(realpath $2)

${DUCKDB_DIR}/build/release/tools/shell/shell ${DB_PATH} < schema.sql
${DUCKDB_DIR}/build/release/tools/shell/shell ${DB_PATH} < load.sql
${DUCKDB_DIR}/build/release/tools/shell/shell ${DB_PATH} <<< "select 1"
