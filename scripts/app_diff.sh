#!/bin/sh

# This script is used to patch an third-party application.
# For example, if you want to create a patch for third_party/fuse/fuse just do:
#    ./scripts/app_diff.sh fuse-2.9.4 build/extbld/third_party/fuse/core third-party/fuse/fuse
# Args:
# $1 - application name
# $2 - path to application within the build folder
# $3 - sources folder

EMBOX_DIR="$(pwd)"
APP_NAME=$1 #fuse-2.9.4
EXTBLD_APP_DIR=$EMBOX_DIR/$2 #build/extbld/third_party/fuse/core
SRC_APP_DIR=$EMBOX_DIR/$3 #third-party/fuse/fuse

cd $EXTBLD_APP_DIR
mkdir -p $APP_NAME-orig
tar xzf $EMBOX_DIR/download/${APP_NAME}.tar.gz -C $APP_NAME-orig --strip-components=1
diff -aur -x configure $APP_NAME-orig $APP_NAME | grep -v "^Only in" | tee $SRC_APP_DIR/patch.txt
rm -rf $APP_NAME-orig
