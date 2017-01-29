#!/usr/bin/env bash
#

if [ $# -ne 1 ]; then
	echo "Usage: `basename $0` path/to/file.img"
	exit $E_BADARGS
fi

IMG_DIR=$(dirname "$1")
IMG_NAME=$(basename "$1")

VDI_DIR=$IMG_DIR
VDI_NAME=${IMG_NAME%.*}.vdi
VDI_UUID="d713cd0d-0dfc-415c-931e-61ab199d4817"

rm -rf $VDI_DIR/$VDI_NAME
VBoxManage convertfromraw --format VDI $IMG_DIR/$IMG_NAME $VDI_DIR/$VDI_NAME
VBoxManage internalcommands sethduuid $VDI_DIR/$VDI_NAME $VDI_UUID
chmod 777 $VDI_DIR/$VDI_NAME
