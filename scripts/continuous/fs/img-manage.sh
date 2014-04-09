#!/bin/sh

FS=$1
IMG=$2
ACTION=$3
BASE_DIR=$(dirname $0)
WORK_DIR=$BASE_DIR
SUDO=sudo

MOUNT_POINT=$WORK_DIR/mountpoint

build_support="vfat ext2 ext3 ext4 iso9660 jffs2"
mount_support="vfat ext2 ext3 ext4"

do_mount() {
	$SUDO mount $IMG -t $FS $MOUNT_POINT
}

do_unmount() {
	sudo umount $MOUNT_POINT
}

is_inlist() {
	echo $2 | grep $1 >/dev/null
}

mk_mountpoint() {
	[ -d $MOUNT_POINT ] || mkdir $MOUNT_POINT
}

build_dir() {
	src=$1
	dst=$2

	if [ ! -d $dst ]; then
		$SUDO mkdir -p $dst
	fi

	if [ nfs == $FS ]; then
		$SUDO chmod 777 $dst
	fi

	$SUDO rm -R $dst/*
	$SUDO cp -aR $src/* $dst
}

build() {
	content=$1

	case $FS in
		jffs2)
			mkfs.jffs2 -p 0x10000 -r $content -o $IMG
			return 0
			;;
		iso9660)
			mkisofs -o $IMG $content
			return 0
			;;
	esac

	dd if=/dev/zero bs=1M count=8 of=$IMG

	case $FS in
		ext?)
			mkfs.$FS -F -b 1024 $IMG
			;;
		*)
			mkfs.$FS $IMG
			;;
	esac

	mk_mountpoint
	do_mount

	build_dir $content $MOUNT_POINT

	do_unmount
}

check_dir() {
	src=$1
	dst=$2

	diff -q -r $src $dst
	ret=$?
	echo img-manage: diff code $ret
	return $ret
}

check() {
	content=$1

	is_inlist $FS "$mount_support" || ( echo "Warning $FS check is not supported";  return 0 )

	mk_mountpoint
	do_mount

	check_dir $content $MOUNT_POINT
	ret=$?

	do_unmount

	exit $ret
}

case $ACTION in
	build_dir) 	build_dir $4 $2 ;;
	build) 		build $4 ;;
	check_dir) 	check_dir $4 $2;;
	check) 		check $4 ;;
	write_content)	write_content $4 ;;
esac
