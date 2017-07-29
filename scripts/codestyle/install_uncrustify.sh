#!/bin/sh

set -xe

RELEASE=0.65

mkdir -p $SRC
cd $SRC

wget -c https://github.com/uncrustify/uncrustify/archive/uncrustify-${RELEASE}.tar.gz
tar axf uncrustify-${RELEASE}.tar.gz

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL ../uncrustify-uncrustify-${RELEASE}

make install
