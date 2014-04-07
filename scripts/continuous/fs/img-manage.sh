
FS=$1
BASE_DIR=$PWD
WORK_DIR=$PWD
SUDO=sudo

shift

MOUNT_POINT=$WORK_DIR/mountpoint

IMG_RO_CONTENT=$BASE_DIR/img-ro
IMG_RW_CONTENT=$BASE_DIR/img-rw
IMG_RW_GOLD=$BASE_DIR/img-rw-gold

IMG=$FS.img

writeable_fs=([fat]=1)

do_mount() {
	$SUDO mount $IMG -t $FS $MOUNT_POINT
}

do_unmount() {
	sudo umount $MOUNT_POINT
}

fs_iswriteable() {
	[ ! -z ${writeable_fs[$FS]} ]
}

build() {
	dd if=/dev/zero bs=1M count=8 of=$IMG
	mkfs.$FS $IMG

	if fs_iswriteable; then
		content=$IMG_RW_CONTENT
	else
		content=$IMG_RO_CONTENT
	fi

	do_mount

	$SUDO cp -R $content/* $MOUNT_POINT

	do_unmount
}

check() {
	do_mount

	if fs_iswriteable; then
		content=$IMG_RW_GOLD
	else
		content=$IMG_RO_CONTENT
	fi

	diff -q -r $content $MOUNT_POINT
	ret=$?

	do_unmount

	exit $ret
}

eval $@
