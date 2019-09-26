#!/bin/bash

# the trailing separators cause a lot of warning logs
./bin/sqlite3 "$1" < schema.sql
./bin/sqlite3 "$1" < load.sql 2>/dev/null
