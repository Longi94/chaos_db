#!/bin/bash
ZIP_SOURCE=sqlite-amalgamation-3290000

wget -O $ZIP_SOURCE.zip https://www.sqlite.org/2019/$ZIP_SOURCE.zip
unzip $ZIP_SOURCE.zip
mkdir bin
gcc $ZIP_SOURCE/shell.c $ZIP_SOURCE/sqlite3.c -lpthread -ldl -o bin/sqlite3

rm $ZIP_SOURCE.zip
rm -rf $ZIP_SOURCE
