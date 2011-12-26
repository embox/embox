#!/bin/bash

PRGNAM=embox
VERSION=0.2.4

pushd ../.. > /dev/null

rm -fr `find $PRGNAM-$VERSION -name ".svn"`
tar cf - $PRGNAM-$VERSION | gzip -f9 > $PRGNAM-$VERSION.tgz
tar cf - $PRGNAM-$VERSION | xz -f > $PRGNAM-$VERSION.txz

popd > /dev/null
