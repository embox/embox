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

TESTABLES="x86/nonvga_debug x86/smp mips/debug ppc/debug microblaze/petalogix \
	sparc/debug x86/test_fs"
#"sparc/qemu" not supported due qemu bug

if ! echo $TESTABLES | grep $ATML &>/dev/null; then
	echo Warning: template testing not supported >&2
	exit 0
fi

TIMEOUT=45
EMKERNEL=./build/base/bin/embox
OUTPUT_FILE=./cont.out

do_it() {
	echo "$@" | sh
}

export AUTOQEMU_KVM_ARG=
export AUTOQEMU_NOGRAPHIC_ARG=
export AUTOQEMU_NICS=
QEMU_COMMON="./scripts/qemu/auto_qemu -serial file:${OUTPUT_FILE} -display none"

declare -A atml2sim
atml2sim['x86/smp']="$QEMU_COMMON -smp 2"
atml2sim['sparc/debug']="tsim-leon3 -c $(dirname $0)/tsim_run.cmd $EMKERNEL \
	> $OUTPUT_FILE"

CMD_LINE="${atml2sim[$ATML]}"
if [ -z $CMD_LINE ]; then
	CMD_LINE="$QEMU_COMMON"
fi

# qemu refuses to write if run with -serial stdio. So we buffer to file,
# cat it, and analyze

[ -f $OUTPUT_FILE ] && rm $OUTPUT_FILE
do_it timeout -s 9 $TIMEOUT "$CMD_LINE $2"

cat $OUTPUT_FILE

for fail_pattern in "fail" "assert"; do
	grep "$fail_pattern" $OUTPUT_FILE &>/dev/null && exit 1
done

for success_pattern in "Started shell" "embox>"; do
	grep "$success_pattern" $OUTPUT_FILE &>/dev/null && exit 0
done

exit 1

