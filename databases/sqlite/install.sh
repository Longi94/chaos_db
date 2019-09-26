#!/bin/bash
ZIP_FILE_NAME=sqlite-tools-linux-x86-3290000

wget -O $ZIP_FILE_NAME.zip https://www.sqlite.org/2019/$ZIP_FILE_NAME.zip
unzip $ZIP_FILE_NAME.zip
mv $ZIP_FILE_NAME bin
rm $ZIP_FILE_NAME.zip

echo ".headers ON" > ~/.sqliterc
