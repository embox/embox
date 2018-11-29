#!/usr/bin/env bash
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
OTHER_ARGS="$@"

TIMEOUT=${CONTINIOUS_RUN_TIMEOUT-45}

EMCONF=./conf
EMKERNEL=./build/base/bin/embox
OUTPUT_FILE=./cont.out

# qemu refuses to write if run with -serial stdio. So we buffer to file,
# cat it, and analyze
AUTOQEMU_KVM_ARG=
AUTOQEMU_NOGRAPHIC_ARG="-serial file:${OUTPUT_FILE} -display none"
RUN_QEMU="./scripts/qemu/auto_qemu $SIM_ARG"

USERMODE_START_OUTPUT=$OUTPUT_FILE

packetdrill_run() {
	AUTOQEMU_NICS=""
	TIMEOUT=120
	default_run
}

declare -A atml2run
atml2run=(
	['arm/qemu']=default_run
	['x86/qemu']=default_run
	['x86/smp']=default_run
	['x86/user_apps']=default_run
	['x86/test/lang']=default_run
	['x86/test/fs']="$(dirname $0)/fs/run.sh $ATML"
	['x86/test/net']="$(dirname $0)/net/run.sh $ATML"
	['x86/test/packetdrill']=packetdrill_run
	['sparc/qemu']=default_run
	['mips/qemu']=default_run
	['ppc/qemu']=default_run
	['microblaze/qemu']=default_run
	['usermode86/debug']=default_run
	['generic/qemu']=default_run
	['generic/qemu_bg']=run_bg_wrapper
	['generic/qemu_bg_kill']=kill_bg_wrapper
	['generic/save_conf']=save_conf
	['generic/restore_conf']=restore_conf
	['generic/fail']=false
)

sudo_var_pass() {
	if [ ${!1+defined} ]; then
		echo $1=${!1}
	else
		#output is passed to sudo that not likes empty arguments
		echo __T=
	fi
}

sim_bg=
run_bg() {
	declare -A atml2sim
	#"sparc/qemu" not supported due qemu bug
	atml2sim=(
		['x86/smp']="$RUN_QEMU -smp 2"
		['sparc/debug']="$(dirname $0)/tsim_run.sh $OUTPUT_FILE $SIM_ARG $EMKERNEL"
		['usermode86/debug']="$(dirname $0)/../usermode_start.sh"
	)

	run_cmd=${atml2sim[$ATML]}
	if [ -z "$run_cmd" ]; then
		run_cmd="$RUN_QEMU"
	fi

	set +ve
	sudo PATH=$PATH \
		AUTOQEMU_KVM_ARG="$AUTOQEMU_KVM_ARG" \
		AUTOQEMU_NOGRAPHIC_ARG="$AUTOQEMU_NOGRAPHIC_ARG" \
		"$(sudo_var_pass AUTOQEMU_NICS)" \
		"$(sudo_var_pass AUTOQEMU_NICS_CONFIG)"	\
		"$(sudo_var_pass KERNEL)" \
		USERMODE_START_OUTPUT="$USERMODE_START_OUTPUT" \
		$run_cmd &
	sim_bg=$!
}

run_check() {

	sudo chmod 666 $OUTPUT_FILE

	ret=1
	for success_pattern in '^run: success auto poweroff' 'embox>' '[a-z]\+@embox'; do
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

kill_bg() {
	pstree -A -p $sim_bg | sed 's/[0-9a-z{}_\.+`-]*(\([0-9]\+\))/\1 /g' | xargs sudo kill

	cat $OUTPUT_FILE

	restore_conf
}

## FIXME not working
#wait_bg() {
#	timeout -s 9 $TIMEOUT wait $sim_bg #wait is builtin, so can't be used as timeout arg
#	if [ 124 -eq $? ]; then
#		kill_bg
#	fi
#}

wait_bg() {
	sleep $TIMEOUT
	kill_bg
}

default_run() {

	run_bg

	wait_bg

	run_check
	ret=$?

	rm $OUTPUT_FILE

	return $ret
}

run_bg_wrapper() {

	run_bg

	sleep $TIMEOUT

	run_check
	ret=$?

	echo "====================="
	echo "Embox output on start"
	echo "====================="
	cat $OUTPUT_FILE

	if [ 0 -ne $ret ]; then
		kill_bg
		exit $ret
	fi

	echo $sim_bg > $OTHER_ARGS
	return 0
}

kill_bg_wrapper() {
	sim_bg=$(cat $OTHER_ARGS)
	echo "==================="
	echo "Embox output on end"
	echo "==================="
	kill_bg
}

save_conf() {
	echo "Save conf/ to $EMCONF.orig"
	rm -rf $EMCONF.orig
	cp -r $EMCONF $EMCONF.orig
}

restore_conf() {
	if [ -d $EMCONF.orig ]; then
		echo "Restore conf/ from $EMCONF.orig"
		cp -rf $EMCONF.orig/* $EMCONF
	fi
}

if ! echo ${!atml2run[@]} | grep $ATML &>/dev/null; then
	echo Warning: template testing not supported >&2
	exit 0
fi

${atml2run[$ATML]}


