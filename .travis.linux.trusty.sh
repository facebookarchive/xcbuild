#! /bin/bash

FILE=libpng-1.6.24.tar.gz
URL=ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/$FILE
MD5SUM=65213080dd30a9b16193d9b83adc1ee9

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
