#!/usr/bin/env bash

# This script is used to patch third-party applications.
# For example, if you want to create a patch for third_party/fuse/fuse just do:
#    ./scripts/app_diff.sh download/fuse-2.9.4.tar.gz build/extbld/third_party/fuse/core/fuse-2.9.4
# Args:
# $1 - path to original archive
# $2 - path to application in build/extbld folder
#
# You can also export PATCHES_APPLY_BEFORE="patch1.txt patch2.txt".
# For example PATCHES_APPLY_BEFORE=third_party/fuse/patch1.txt:third_party/fuse/patch2.txt
# to apply existing patches before
#

ROOT_DIR=$PWD
APP_ARCHIVE_PATH=$1
APP_PATH=$2

print_usage() {
	echo -e "Usage:"
	echo -e "  ./scripts/app_diff.sh <APP_ARCHIVE_PATH> <APP_PATH>"
	echo -e "  Example:"
	echo -e "    ./scripts/app_diff.sh download/fuse-2.9.4.tar.gz build/extbld/third_party/fuse/core/fuse-2.9.4\n"
	echo -e "You can also export PATCHES_APPLY_BEFORE to apply some patches before:"
	echo -e "  export PATCHES_APPLY_BEFORE=third_party/fuse/patch1.txt:third_party/fuse/patch2.txt"
}

if [ -z $APP_ARCHIVE_PATH ] || [ -z $APP_PATH ]; then
	print_usage
	exit 1
fi

shift 2
DIFF_OPTIONS=$@

EXTRACT_FOLDER=.orig_archive

mkdir -p $EXTRACT_FOLDER

ARCHIVE_BASENAME=$(basename $APP_ARCHIVE_PATH)
case $ARCHIVE_BASENAME in
	*tar|*tar.gz|*tar.xz|*tar.bz2|*tgz|*tbz)
		tar -xf $APP_ARCHIVE_PATH -C $EXTRACT_FOLDER ;;
	*zip)
		unzip -qq $APP_ARCHIVE_PATH -d $EXTRACT_FOLDER ;;
	*)
		echo "Unsupported archive extension"; exit 1 ;;
esac

pushd $EXTRACT_FOLDER > /dev/null

# Apply patches if any
if [ ! -z "$PATCHES_APPLY_BEFORE" ]; then
	IFS=:
	for i in ${PATCHES_APPLY_BEFORE}
	do
		patch -s -p0 < $ROOT_DIR/$i
	done
fi
LC_ALL=C diff -aur $DIFF_OPTIONS * ../$APP_PATH | grep -v '^Only in'

popd > /dev/null

rm -rf $EXTRACT_FOLDER
