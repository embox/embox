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
ARCH=$(echo $ATML | cut -d \/ -f 1)

TESTABLES="x86/nonvga_debug x86/smp mips/debug ppc/debug microblaze/petalogix \
	sparc/debug"
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

declare -A atml2sim

export AUTOQEMU_KVM_ARG=
export AUTOQEMU_NOGRAPHIC_ARG=
export AUTOQEMU_NICS=
QEMU_COMMON="./scripts/qemu/auto_qemu -serial file:${OUTPUT_FILE} -display none"

atml2sim['x86/nonvga_debug']="$QEMU_COMMON"
atml2sim['x86/smp']="$QEMU_COMMON -smp 2"
atml2sim['mips/debug']="$QEMU_COMMON"
atml2sim['ppc/debug']="$QEMU_COMMON"
atml2sim['microblaze/petalogix']="$QEMU_COMMON"
atml2sim['sparc/qemu']="$QEMU_COMMON"

atml2sim['sparc/debug']="tsim-leon3 -c $(dirname $0)/tsim_run.cmd $EMKERNEL \
	> $OUTPUT_FILE"

# qemu refuses to write if run with -serial stdio. So we buffer to file,
# cat it, and analyze

[ -f $OUTPUT_FILE ] && rm $OUTPUT_FILE
do_it timeout -s 9 $TIMEOUT "${atml2sim[$ATML]}"

cat $OUTPUT_FILE

for success_pattern in "Started shell" "embox>"; do
	grep "$success_pattern" $OUTPUT_FILE &>/dev/null && exit 0
done

exit 1

