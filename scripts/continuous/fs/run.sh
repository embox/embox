
ROOT_DIR=.
BASE_DIR=$(dirname $0)


IMGS=$(ls $BASE_DIR/*.img)
START_SCRIPT=$ROOT_DIR/conf/start_script.inc

start_script_store() {
	cp $START_SCRIPT $START_SCRIPT.old
}

start_script_restore() {
	mv $START_SCRIPT.old $START_SCRIPT
}

check_exit() {
	ret=$?
	if [ 0 -ne $ret ]; then
		start_script_restore
		exit $ret
	fi
}

for i in $IMGS; do
	cp $START_SCRIPT $START_SCRIPT.old

	echo \"mount -t $(basename ${i%.img}) /dev/hda /mnt/fs_test\", >> $START_SCRIPT
	echo \"test -t fs_test\", >> $START_SCRIPT
	echo \"umount /mnt/fs_test\", >> $START_SCRIPT

	make &> /dev/null
	check_exit

	#$BASE_DIR/../run.sh x86/test_fs "-hda $i"

	./scripts/qemu/auto_qemu -hda $i
	check_exit

	start_script_restore
done

