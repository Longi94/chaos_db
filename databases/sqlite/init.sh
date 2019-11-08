#!/bin/bash

set -e

die () {
    echo >&2 "$@"
    exit 1
}

[[ $# -ge 1 ]] || die "1 argument required, $# provided"

python3 ../../orchestrator/generate_sql_file.py -i update.sql -o update.sql.tmp

# the trailing separators cause a lot of warning logs
./bin/sqlite3 "$1" < schema.sql
./bin/sqlite3 "$1" < load.sql 2>/dev/null
