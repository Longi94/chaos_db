#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 2 ]] || die "2 argument required, $# provided"

SQLITE_DIR=$(realpath $1)

python3 ../../orchestrator/generate_sql_file.py -i update.sql -o update.sql.tmp

# the trailing separators cause a lot of warning logs
${SQLITE_DIR}/bin/sqlite3 "$1" < schema.sql
${SQLITE_DIR}/bin/sqlite3 "$1" < load.sql 2>/dev/null
