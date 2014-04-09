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

TIMEOUT=45
EMKERNEL=./build/base/bin/embox
OUTPUT_FILE=./cont.out

# qemu refuses to write if run with -serial stdio. So we buffer to file,
# cat it, and analyze
AUTOQEMU_KVM_ARG=
AUTOQEMU_NOGRAPHIC_ARG="-serial file:${OUTPUT_FILE} -display none"
RUN_QEMU="./scripts/qemu/auto_qemu $SIM_ARG"

declare -A atml2run
atml2run=(
	['x86/nonvga_debug']=default_run
	['x86/smp']=default_run
	['x86/test_fs']="$(dirname $0)/fs/run.sh $ATML"
	['sparc/debug']=default_run
	['mips/debug']=default_run
	['ppc/debug']=default_run
	['microblaze/petalogix']=default_run
	['generic/qemu']=default_run
	['generic/fail']=false
)

do_it() {
	echo "$@" | sh
}

default_run() {

	declare -A atml2sim
	#"sparc/qemu" not supported due qemu bug
	atml2sim=(
		['x86/nonvga_debug']="$RUN_QEMU"
		['x86/smp']="$RUN_QEMU -smp 2"
		['sparc/debug']="tsim-leon3 -c $(dirname $0)/tsim_run.cmd $SIM_ARG $EMKERNEL > $OUTPUT_FILE"
		['mips/debug']="$RUN_QEMU"
		['ppc/debug']="$RUN_QEMU"
		['microblaze/petalogix']="$RUN_QEMU"
		['generic/qemu']="$RUN_QEMU"
	)

	[ -f $OUTPUT_FILE ] && rm -f $OUTPUT_FILE

	do_it sudo "AUTOQEMU_KVM_ARG=\"$AUTOQEMU_KVM_ARG\"" \
		"AUTOQEMU_NOGRAPHIC_ARG=\"$AUTOQEMU_NOGRAPHIC_ARG\"" \
		timeout $TIMEOUT "${atml2sim[$ATML]}"

	cat $OUTPUT_FILE

	for fail_pattern in "fail" "assert"; do
		grep "$fail_pattern" $OUTPUT_FILE &>/dev/null && exit 1
	done

	for success_pattern in '^embox>' '^[a-z]\+@embox'; do
		grep "$success_pattern" $OUTPUT_FILE &>/dev/null && exit 0
	done

	exit 1
}

if ! echo ${!atml2run[@]} | grep $ATML &>/dev/null; then
	echo Warning: template testing not supported >&2
	exit 0
fi

${atml2run[$ATML]}


