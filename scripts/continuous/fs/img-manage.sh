#!/bin/sh

FS=$1
IMG=$2
ACTION=$3
BASE_DIR=$(dirname $0)
WORK_DIR=$BASE_DIR
SUDO=sudo

MOUNT_POINT=$WORK_DIR/mountpoint

build_support="vfat ext2 ext3 ext4 iso9660 jffs2 ntfs"
mount_support="vfat ext2 ext3 ext4 ntfs"

do_mount() {
	$SUDO mount $IMG -t $FS $MOUNT_POINT
}

do_unmount() {
	$SUDO umount -f $MOUNT_POINT
}

is_inlist() {
	echo $2 | grep $1 >/dev/null
}

mk_mountpoint() {
	[ -d $MOUNT_POINT ] || mkdir $MOUNT_POINT
}

build_dir() {
	srcs="$1"
	dst=$2

	if [ ! -d $dst ]; then
		$SUDO mkdir -p $dst
	fi

	$SUDO rm -R $dst/* 2>/dev/null

	for s in $srcs; do
		$SUDO cp -aR $s/* $dst
	done

	$SUDO find $dst -name .\* -exec rm -Rf {} +

	# Only nfs deals with real uids, bypassing possible 'access denied's
	if [ nfs = $FS ]; then
		$SUDO chmod -R 777 $dst
	fi
}

build() {
	content="$1"
	tmp_dir=tmp_$FS

	if [ jffs2 = $FS ] || [ iso9660 = $FS ]; then
		mkdir -p $tmp_dir

		build_dir "$content" $tmp_dir

		case $FS in
			jffs2)
				$SUDO mkfs.jffs2 -p 0x10000 -r $tmp_dir -o $IMG
				;;
			iso9660)
				$SUDO mkisofs -o $IMG $tmp_dir
				;;
		esac

		rm -Rf $tmp_dir
		return 0
	fi

	dd if=/dev/zero bs=1M count=8 of=$IMG

	case $FS in
		ext?)
			$SUDO mkfs.$FS -F -b 1024 $IMG
			;;
		ntfs)
			$SUDO mkntfs -F $IMG
			;;
		*)
			$SUDO mkfs.$FS $IMG
			;;
	esac

	mk_mountpoint
	do_mount

	build_dir "$content" $MOUNT_POINT

	do_unmount
}

check_dir() {
	src=$1
	dst=$2

	diff -u -r --exclude=".*" $src $dst
	ret=$?
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
	build_dir) 	build_dir "$4" $2 ;;
	build) 		build "$4" ;;
	check_dir) 	check_dir $4 $2;;
	check) 		check $4 ;;
	write_content)	write_content $4 ;;
esac
