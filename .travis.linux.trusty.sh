#! /bin/bash

URL=ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/libpng-1.6.23.tar.gz
FILE=libpng-1.6.23.tar.gz
MD5SUM=a49e4cc48d968c79def53d082809c9f2

echo Downloading $URL
curl $URL -o $FILE
echo "$MD5SUM  $FILE" | md5sum -c
if [ "$?" != "0" ]; then
    exit 1
fi

echo Extracting
tar -zxvvf $FILE
cd ${FILE%.tar.gz}

echo Running configure/make/make install
./configure --prefix=/usr/local
make
make install
