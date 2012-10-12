#!/bin/bash
# Gotta have a grub 0.97-like installed on your system
#

EMBOX_BIN_DIR="/home/user/Projects/embox/build/base/bin"
EMBOX_BIN="embox"
VDI_OUTPUT_DIR="/home/user"
VDI_OUTPUT_IMG="embox.vdi"
TEMP_DIR="/tmp"
TEMP_IMG="embox_vdi_temp.img"
DD_IMAGE_SIZE=40320
DD_IMAGE_OFFSET=32256
MOUNT_DIR="/mnt/embox"
GRUB_DIR="/boot/grub"
LOOP_DEVICE="/dev/loop0"

if [ "$(id -u)" != "0" ]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo -e "\n\nTrying to make up an empty disk image\n\n"
dd if=/dev/zero of=$TEMP_DIR/$TEMP_IMG count=$DD_IMAGE_SIZE

echo -e "\n\nMake a new pertition on image\n\n"
echo -e "x\nh\n16\ns\n63\nc\n40\nr\nn\np\n1\n\n\nw\n\n\n" | fdisk $TEMP_DIR/$TEMP_IMG

echo -e "\n\nMake a file system\n\n"
losetup -o $DD_IMAGE_OFFSET $LOOP_DEVICE $TEMP_DIR/$TEMP_IMG
mke2fs $LOOP_DEVICE
losetup -d $LOOP_DEVICE

echo -e "\n\nMount image\n\n"
mkdir $MOUNT_DIR
mount -o loop,offset=$DD_IMAGE_OFFSET $TEMP_DIR/$TEMP_IMG $MOUNT_DIR

echo -e "\n\nMoving files to image\n\n"
mkdir -p $MOUNT_DIR$GRUB_DIR
cp $EMBOX_BIN_DIR/$EMBOX_BIN $MOUNT_DIR
cp $GRUB_DIR/{stage1,stage2,e2fs_stage1_5} $MOUNT_DIR$GRUB_DIR
umount $TEMP_DIR/$TEMP_IMG

echo -e "\n\nInstall grub\n\n"
echo -e "device (hd0) $TEMP_DIR/$TEMP_IMG\ngeometry (hd0) 40 16 63\nroot (hd0,0)\nsetup (hd0)\nquit\n\n" | grub --device-map=/dev/null

echo -e "\n\nConvert image to VDI\n\n"
rm -f $VDI_OUTPUT_DIR/$VDI_OUTPUT_IMG
VBoxManage convertfromraw --format VDI $TEMP_DIR/$TEMP_IMG $VDI_OUTPUT_DIR/$VDI_OUTPUT_IMG
chmod 777 $VDI_OUTPUT_DIR/$VDI_OUTPUT_IMG

rm -f $TEMP_DIR/$TEMP_IMG

