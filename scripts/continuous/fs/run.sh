#!/bin/sh

#FS_TEST_RO="iso9660 jffs2"
#FS_TEST_RW="vfat ext2 ext3 ext4"
FS_TEST_RO="jffs2 ext4"
FS_TEST_RW="ext2 ext3 ntfs vfat"
FS_TEST_NETWORK="nfs cifs"

FS_TEST_NFS_ROOT="/var/nfs_test"

FS_TEST_CIFS_SHARE="/Public"
FS_TEST_CIFS_PATH="/var/cifs_test"

ROOT_DIR=.
BASE_DIR=$ROOT_DIR
DATA_DIR=$(dirname $0)

START_SCRIPT=$ROOT_DIR/conf/start_script.inc
CONT_BASE=$ROOT_DIR/scripts/continuous
CONT_FS_MANAGE=$CONT_BASE/fs/img-manage.sh
CONT_RUN=$CONT_BASE/run.sh
PID_FILE=$BASE_DIR/qemu_bg.pid

IMG_RO_CONTENT=$DATA_DIR/img-ro
IMG_RW_CONTENT="$DATA_DIR/img-rw $IMG_RO_CONTENT"
IMG_RW_GOLD=$DATA_DIR/img-rw-gold

EXPECT_TESTS_BASE=$ROOT_DIR/scripts/expect

posted_ret=0
check_post_exit() {
	ret=$?
	if [ 0 -ne $ret ]; then
		echo - - - - - - - - - - - - - - -
		echo ERROR: $1
		echo - - - - - - - - - - - - - - -

		posted_ret=$ret
	fi
}

qemu_changed_startscript=
run_qemu_fs() {
	FS=$1
	IMG=$2
	RW=$3

	cp $START_SCRIPT $START_SCRIPT.old
	qemu_changed_startscript=1

	img_mount=
	QEMU_MOUNT_HD="\"mount -t $FS /dev/hda /mnt/fs_test\","
	case $FS in
		vfat | ext2 | ext3 | ext4 | ntfs)
			img_mount="$QEMU_MOUNT_HD"
			;;
		jffs2)
			img_mount="\"mkfs -t $FS /dev/hda\",$QEMU_MOUNT_HD"
			;;
		iso9660)
			img_mount="\"mount -t $FS /dev/cd0 /mnt/fs_test\","
			;;
		nfs)
			img_mount="\"mount -t $FS 10.0.2.10:$IMG /mnt/fs_test\","
			;;
		cifs)
			img_mount="\"mount -t $FS 10.0.2.10$IMG /mnt/fs_test\","
			;;
	esac

	echo $img_mount >> $START_SCRIPT

	echo \"ls /mnt/fs_test\", >> $START_SCRIPT
	echo \"test -t fs_test_read\", >> $START_SCRIPT
	if [ rw = $RW ]; then
		echo \"test -t fs_test_write\", >> $START_SCRIPT
	fi

	echo \"umount /mnt/fs_test\", >> $START_SCRIPT

	make >/dev/null 2>/dev/null

	img_run=
	case $FS in
		vfat | ext2 | ext3 | ext4 | jffs2 | ntfs)
			img_run="-hda $IMG"
			;;
		iso9660)
			img_run="-cdrom $IMG"
			;;
		cifs | nfs)
			;;
	esac

	$CONT_RUN generic/qemu "$img_run"
	#./scripts/qemu/auto_qemu $img_run
	check_post_exit "qemu run failed"

	mv -f $START_SCRIPT.old $START_SCRIPT
}

run_qemu_cleanup() {
	if [ ! -z $qemu_changed_startscript ]; then
		make >/dev/null 2>/dev/null
	fi
}

banner() {
	fs="$1"

	echo  ================================
	echo Starting test "$fs" filesystem
	echo  ================================
}

interactive_tests() {
	EXPECT_TEST_CONFIG=$1

	$CONT_RUN generic/qemu_bg "" $PID_FILE

	expect $EXPECT_TESTS_BASE/framework/run_all.exp \
		$EXPECT_TEST_CONFIG 10.0.2.16 10.0.2.10 ""

	check_post_exit "Interactive tests failed"

	$CONT_RUN generic/qemu_bg_kill "" $PID_FILE
	rm $PID_FILE
}

interactive_tests $EXPECT_TESTS_BASE/x86_fs_unit_tests.config
interactive_tests $EXPECT_TESTS_BASE/x86_fs_shell_commands.config

for f in $FS_TEST_RO; do
	img=$BASE_DIR/$f.img

	banner "$f (ro)"
	$CONT_FS_MANAGE $f $img build $IMG_RO_CONTENT

	run_qemu_fs $f $img "ro"
done

for f in $FS_TEST_RW; do
	img=$BASE_DIR/$f.img
	img_work=$img.work

	banner "$f (rw)"

	$CONT_FS_MANAGE $f $img build "$IMG_RW_CONTENT"

	cp $img $img_work

	run_qemu_fs $f $img_work "rw"

	$CONT_FS_MANAGE $f $img_work check $IMG_RW_GOLD
	check_post_exit "fs content differ from expected"

	rm $img_work
done

for f in $FS_TEST_NETWORK; do
	banner "$f (net)"

	case $f in
		nfs)
			sudo mount -t tmpfs none $FS_TEST_NFS_ROOT
			sudo service rpcbind restart
			sudo /etc/init.d/nfs-kernel-server restart

			$CONT_FS_MANAGE $f $FS_TEST_NFS_ROOT build_dir "$IMG_RW_CONTENT"

			run_qemu_fs $f $FS_TEST_NFS_ROOT "rw"

			$CONT_FS_MANAGE $f $FS_TEST_NFS_ROOT check_dir $IMG_RW_GOLD
			check_post_exit "fs content differ from expected"
			;;
		cifs)
			sudo /etc/init.d/nmbd restart
			sudo /etc/init.d/smbd restart

			$CONT_FS_MANAGE $f $FS_TEST_CIFS_PATH build_dir "$IMG_RW_CONTENT"

			run_qemu_fs $f $FS_TEST_CIFS_SHARE "rw"

			$CONT_FS_MANAGE $f $FS_TEST_CIFS_ROOT check_dir $IMG_RW_GOLD
			check_post_exit "fs content differ from expected"
			;;
	esac
done

run_qemu_cleanup

exit $posted_ret
