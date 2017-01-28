#!/usr/bin/env bash
# Gotta have a grub 0.97-like installed on your system
#

cd "$(dirname "$0")"

KERNEL_NAME="embox"
BIN_DIR="../../build/base/bin"

IMG_NAME="$KERNEL_NAME.img"
IMG_DIR=$BIN_DIR
MOUNT_DIR="/mnt/$KERNEL_NAME"

GRUB_FILES="grub_files/*"
GRUB_DIR="/boot/grub"
DD_IMAGE_SIZE=40320
DD_IMAGE_OFFSET=1048576
LOOP_DEVICE="/dev/loop0"
LOG_FILE="error.log"

if [ "$(id -u)" != "0" ]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

function title {
	echo "$1" | tee -a $LOG_FILE
}

function call {
	echo "$ $1" | tee -a $LOG_FILE
	$1 &>>$LOG_FILE || error
}

function error {
	echo "There was an error! See $LOG_FILE" 1>&2
	exit 1
}

rm -rf $LOG_FILE

title "Trying to make up an empty disk image"
call "dd if=/dev/zero of=$IMG_DIR/$IMG_NAME count=$DD_IMAGE_SIZE"
call "chmod 777 $IMG_DIR/$IMG_NAME"

title "Make a new pertition on image"
echo -e "x\nh\n16\ns\n63\nc\n40\nr\nn\np\n1\n\n\na\n1\nw\n" | call "fdisk -c -u $IMG_DIR/$IMG_NAME"

title "Setup loopback device"
call "fdisk -c -u -l $IMG_DIR/$IMG_NAME"
call "losetup -o $DD_IMAGE_OFFSET $LOOP_DEVICE $IMG_DIR/$IMG_NAME"

title "Make a file system"
call "mkfs.ext2 $LOOP_DEVICE"

title "Mount the image"
call "mkdir -p $MOUNT_DIR"
call "mount -t ext2 $LOOP_DEVICE $MOUNT_DIR"

title "Moving files to image"
call "mkdir -p $MOUNT_DIR/$GRUB_DIR"
call "cp $BIN_DIR/$KERNEL_NAME $MOUNT_DIR"
call "cp $GRUB_FILES $MOUNT_DIR/$GRUB_DIR"

title "Umount the disk image"
call "sync"
call "umount $MOUNT_DIR"
call "rm -rf $MOUNT_DIR"

title "Deatach the loopback device"
call "losetup -d $LOOP_DEVICE"

title "Install grub"
echo -e "device (hd0) $IMG_DIR/$IMG_NAME\ngeometry (hd0) 40 16 63\nroot (hd0,0)\nsetup (hd0)\nquit\n" | call "grub --device-map=/dev/null"

title "Done!"

rm -rf $LOG_FILE
exit 0
