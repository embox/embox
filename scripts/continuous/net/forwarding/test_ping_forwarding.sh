#!/usr/bin/env bash

set -u

ROOT_DIR=.
BASE_DIR=$ROOT_DIR

CONT_BASE=$ROOT_DIR/scripts/continuous
CONT_RUN=$CONT_BASE/run.sh
TEST_DIR=$CONT_BASE/net/forwarding

TAP_DEV=tap0

EMBOX1_START_SCRIPT=$TEST_DIR/embox1_start_script
EMBOX2_START_SCRIPT=$TEST_DIR/embox2_start_script

QEMU1_PID_FILE=$BASE_DIR/qemu_bg1.pid
QEMU2_PID_FILE=$BASE_DIR/qemu_bg2.pid

QEMU1_ETH0=10.0.2.16
QEMU1_ETH1=172.16.0.16
QEMU2_ETH0=172.16.2.10

EMBOX1_KERNEL=$TEST_DIR/embox1
EMBOX2_KERNEL=$TEST_DIR/embox2

test_code=0

build_embox() {
	embox_start_script=$1

	echo "building embox with $embox_start_script"
	cat $embox_start_script > $ROOT_DIR/conf/start_script.inc
	make -j9 > /dev/null 2>&1
	mv $ROOT_DIR/build/base/bin/embox $2
}

test_suite_setup() {
	$CONT_RUN generic/save_conf

	build_embox $EMBOX1_START_SCRIPT $EMBOX1_KERNEL
	build_embox $EMBOX2_START_SCRIPT $EMBOX2_KERNEL

	export CONTINIOUS_RUN_TIMEOUT=120
	AUTOQEMU_NICS="" AUTOQEMU_NICS_CONFIG="" KERNEL=$EMBOX1_KERNEL $CONT_RUN generic/qemu_bg \
		 "-net nic,vlan=0,model=virtio,macaddr=AA:BB:CC:DD:EE:12
			-net tap,vlan=0,script=./scripts/qemu/start_script,downscript=./scripts/qemu/stop_script
			-net nic,model=virtio,vlan=1,macaddr=AA:BB:CC:DD:EE:22 -net socket,vlan=1,listen=:12345" \
		 $QEMU1_PID_FILE

	AUTOQEMU_NICS="" AUTOQEMU_NICS_CONFIG="" KERNEL=$EMBOX2_KERNEL $CONT_RUN generic/qemu_bg \
		 "-net nic,model=virtio,vlan=3,macaddr=AA:BB:CC:DD:EE:23
			-net socket,vlan=3,connect=:12345" \
		 $QEMU2_PID_FILE

	sudo route add default gw $QEMU1_ETH0 dev $TAP_DEV
}

test_suite_teardown() {
	$CONT_RUN generic/restore_conf

	$CONT_RUN generic/qemu_bg_kill "" $QEMU1_PID_FILE || true
	$CONT_RUN generic/qemu_bg_kill "" $QEMU2_PID_FILE || true
	rm $EMBOX1_KERNEL $EMBOX2_KERNEL || true
}

test_retcode() {
	if [ 0 -ne $? ]; then
		test_code=1
	fi
}

test_case_ping_forwarding() {
	echo "Test case \"embox should forward ping\""
	ping -I $TAP_DEV -c 4 $QEMU2_ETH0
	test_retcode
}

test_case_big_ping_forwarding() {
	echo "Test case \"embox should forward big ping\""
	ping -I $TAP_DEV -c 4 -s 16384 $QEMU2_ETH0
	test_retcode
}

test_suite_setup
	test_case_ping_forwarding
	test_case_big_ping_forwarding
test_suite_teardown

exit $test_code

