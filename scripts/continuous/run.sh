#!/bin/bash
# bash is required by declare -A
# @file
# @brief
#
# @author  Anton Kozlov
# @date    30.07.2013
#

# ATML == arch + template, i.e. x86/debug
ATML="$1"
SIM_ARG="$2"
shift 2
OTHER_ARGS=$@

if [ $CONTINIOUS_RUN_TIMEOUT ]; then
	TIMEOUT=$CONTINIOUS_RUN_TIMEOUT
else
	TIMEOUT=45
fi

EMKERNEL=./build/base/bin/embox
OUTPUT_FILE=./cont.out

# qemu refuses to write if run with -serial stdio. So we buffer to file,
# cat it, and analyze
AUTOQEMU_KVM_ARG=
AUTOQEMU_NOGRAPHIC_ARG="-serial file:${OUTPUT_FILE} -display none"
RUN_QEMU="./scripts/qemu/auto_qemu $SIM_ARG"

declare -A atml2run
atml2run=(
	['arm/qemu']=default_run
	['x86/nonvga_debug']=default_run
	['x86/smp']=default_run
	['x86/test_fs']="$(dirname $0)/fs/run.sh $ATML"
	['x86/test_net']="$(dirname $0)/net/run.sh $ATML"
	['sparc/debug']=default_run
	['mips/debug']=default_run
	['ppc/debug']=default_run
	['microblaze/petalogix']=default_run
	['generic/qemu']=default_run
	['generic/qemu_bg']=run_bg_wrapper
	['generic/qemu_bg_kill']=kill_bg_wrapper
	['generic/fail']=false
)

sim_bg=
run_bg() {
	declare -A atml2sim
	#"sparc/qemu" not supported due qemu bug
	atml2sim=(
		['arm/qemu']="$RUN_QEMU"
		['x86/nonvga_debug']="$RUN_QEMU"
		['x86/smp']="$RUN_QEMU -smp 2"
		['sparc/debug']="$(dirname $0)/tsim_run.sh $OUTPUT_FILE $SIM_ARG $EMKERNEL"
		['mips/debug']="$RUN_QEMU"
		['ppc/debug']="$RUN_QEMU"
		['microblaze/petalogix']="$RUN_QEMU"
		['generic/qemu_bg']="$RUN_QEMU"
		['generic/qemu']="$RUN_QEMU"
	)

	sudo PATH=$PATH \
		AUTOQEMU_KVM_ARG="$AUTOQEMU_KVM_ARG" \
		AUTOQEMU_NOGRAPHIC_ARG="$AUTOQEMU_NOGRAPHIC_ARG" \
		AUTOQEMU_NICS_CONFIG="$AUTOQEMU_NICS_CONFIG" \
		${atml2sim[$ATML]} &
	sim_bg=$!
}

run_check() {

	ret=1
	for success_pattern in 'embox>' '^[a-z]\+@embox'; do
		if grep "$success_pattern" $OUTPUT_FILE &>/dev/null ; then
			ret=0
		fi
	done
	for fail_pattern in "fail" "assert"; do
		if grep "$fail_pattern" $OUTPUT_FILE &>/dev/null ; then
			ret=1
		fi
	done

	return $ret
}

run_bg_do() {

	run_bg

	sleep $TIMEOUT

	sudo chmod 666 $OUTPUT_FILE

	cat $OUTPUT_FILE

	run_check
}

kill_bg() {
	pstree -A -p $sim_bg | sed 's/[0-9a-z{}_\.+`-]*(\([0-9]\+\))/\1 /g' | xargs sudo kill
}

default_run() {

	run_bg_do
	ret=$?

	rm $OUTPUT_FILE

	kill_bg

	return $ret
}

run_bg_wrapper() {

	run_bg_do
	ret=$?

	if [ 0 -ne $ret ]; then
		kill_bg
		exit $ret
	fi

	echo $sim_bg > $OTHER_ARGS
	return 0
}

kill_bg_wrapper() {
	sim_bg=$(cat $OTHER_ARGS)
	kill_bg
}

if ! echo ${!atml2run[@]} | grep $ATML &>/dev/null; then
	echo Warning: template testing not supported >&2
	exit 0
fi

${atml2run[$ATML]}


