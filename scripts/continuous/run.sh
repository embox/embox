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
PID_FILE="$3"

TIMEOUT=${CONTINIOUS_RUN_TIMEOUT:-60}

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

x86_smp_run() {
	TIMEOUT=120
	default_run
}

declare -A atml2run
atml2run=(
	['arm/qemu']=default_run
	['arm/stm32f4cube']=true
	['x86/qemu']=default_run
	['x86/smp']=x86_smp_run
	['x86/user_apps']=default_run
	['x86/test/lang']=default_run
	['x86/test/fs']="$(dirname "$0")/fs/run.sh $ATML"
	['x86/test/net']="$(dirname "$0")/net/run.sh $ATML"
	['x86/test/packetdrill']=packetdrill_run
	['x86/test/qt-vnc']="$(dirname "$0")/qt/run.sh $ATML"
	['sparc/qemu']=default_run
	['mips/qemu']=default_run
	['ppc/qemu']=default_run
	['microblaze/qemu']=default_run
#	['usermode86/debug']=default_run
	['generic/qemu']=default_run
	['generic/qemu_bg']="run_bg_wrapper true"
	['generic/qemu_bg_no_check']="run_bg_wrapper false"
	['generic/qemu_bg_kill']=kill_bg_wrapper
	['generic/save_conf']=save_conf
	['generic/restore_conf']=restore_conf
	['generic/fail']=false
)

sudo_var_pass() {
	if [ ${!1+defined} ]; then
		echo "$1=${!1}"
	else
		#output is passed to sudo that not likes empty arguments
		echo __T=
	fi
}

run_check() {
	awk '
		/^run: success auto poweroff/ || /embox>/ || /[a-z]+@embox/ { s = 1 }
		/fail/ || /assert/ { f = 1 }
		END { exit !(f || s) ? 2 : f || !s }
	' $OUTPUT_FILE
}

sim_bg=
run_bg() {
	rm -f $OUTPUT_FILE
	touch $OUTPUT_FILE

	declare -A atml2sim
	#"sparc/qemu" not supported due qemu bug
	atml2sim=(
		['x86/smp']="$RUN_QEMU -smp 2"
		['sparc/debug']="$(dirname "$0")/tsim_run.sh $OUTPUT_FILE $SIM_ARG $EMKERNEL"
		['usermode86/debug']="$(dirname "$0")/../usermode_start.sh"
	)

	run_cmd=${atml2sim[$ATML]}
	if [ -z "$run_cmd" ]; then
		run_cmd="$RUN_QEMU"
	fi

	set +ve
	sudo PATH="$PATH" \
		AUTOQEMU_KVM_ARG="$AUTOQEMU_KVM_ARG" \
		AUTOQEMU_NOGRAPHIC_ARG="$AUTOQEMU_NOGRAPHIC_ARG" \
		"$(sudo_var_pass AUTOQEMU_NICS)" \
		"$(sudo_var_pass AUTOQEMU_NICS_CONFIG)"	\
		"$(sudo_var_pass KERNEL)" \
		USERMODE_START_OUTPUT="$USERMODE_START_OUTPUT" \
		$run_cmd &
	sim_bg=$!

	export OUTPUT_FILE
	export -f run_check
	timeout "$TIMEOUT" bash -c '
			while run_check; [ $? == 2 ]; do
				sleep 1
			done' && \
		sleep 5 # let things to settle down
}

kill_bg() {
	# Sometimes $sim_bg is empty string, so we should make sure pstree is
	# called with acual PID (otherwise it will print every process running)
	if test -z "$sim_bg"
	then
		echo "warning: No background process running"
	else
	        # This quite a cryptic line actually does the following:
	        #   1. It draws a subtree of processes rooted at a process with a PID contained in $sim_bg
	        #   2. Then it cleans resulting lines from everything except processes' PIDs
	        #   3. Finally it kills processes having PIDs it has found
	        # In other words this line kills a process and all its descendents.
		pstree -A -p $sim_bg | sed 's/[0-9a-z{}_\.+`-]*(\([0-9]\+\))/\1 /g' | xargs sudo kill
	fi

	cat $OUTPUT_FILE

	restore_conf
}

default_run() {

	run_bg

	kill_bg

	run_check
	ret=$?

	rm $OUTPUT_FILE

	return $ret
}

run_bg_wrapper() {
	check_if_started=$1

	run_bg

	ret=0
	if [ "$check_if_started" = true ]; then
		run_check
		ret=$?
	fi

	echo "====================="
	echo "Embox output on start"
	echo "====================="
	cat $OUTPUT_FILE

	if [ 0 -ne $ret ]; then
		kill_bg
		exit $ret
	fi

	echo $sim_bg > "$PID_FILE"
	return 0
}

kill_bg_wrapper() {
	sim_bg=$(cat "$PID_FILE")
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

if ! [ -v "atml2run[$ATML]" ]; then
	echo Warning: template testing not supported >&2
	exit 0
fi

${atml2run[$ATML]}


