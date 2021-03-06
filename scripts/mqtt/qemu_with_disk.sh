#!/usr/bin/env bash

DATADIR="$(dirname "$0")"
CONFDIR=./conf

KERNEL=./build/base/bin/embox
MODS_CONF=$CONFDIR/mods.config
LDS_CONF=$CONFDIR/lds.conf
BUILD_CONF=$CONFDIR/build.conf

ARCHs=()
NICs=()

info() {
	echo $@ >&2
}

error() {
	echo $@ >&2
	exit 1
}

guessed_info() {
	info Guessed $1 is: ${!1}
}

error_empty() {
	if [ -z "${!1}" ]; then
		error $1 is not defined
	fi
}

get_user_empty_guess() {
	if [ -z "${!1}" ]; then
		eval "$1=\"$($2)\""
		guessed_info "$1"
		error_empty "$1"
	fi
}

get_user_undef_guess() {
	if [ -z "${!1+defined}" ]; then
		eval "$1=\"$($2)\""
		guessed_info $1
	fi
}

guess_arch() { sed -n 's/ARCH *= *//p' $BUILD_CONF; }
get_user_empty_guess AUTOQEMU_ARCH guess_arch

guess_mem() {
	ram=$(sed -n 's/^RAM *([0-9x]*, *\([0-9]*\)M)/\1/p' $LDS_CONF)
	if [ $ram ]; then
		echo $ram
	else
		echo 128
	fi
}
get_user_empty_guess AUTOQEMU_MEM guess_mem

guess_nics() { sed -n 's/\t*\(@Runlevel([0-9])\|\) \+include\ embox.driver.net.\([a-z_-]\)/\2/p' $MODS_CONF | grep -v loopback | head -n 1; }
get_user_undef_guess AUTOQEMU_NICS guess_nics

guess_kvm() {
	ret=""
	# Build kvm argument
	if [ $AUTOQEMU_ARCH = "x86" ]; then
		egrep '(vmx|svm)' /proc/cpuinfo > /dev/null
		if [ $? -eq 0 ]; then
			ret="-enable-kvm"
		else
			info VT is not supported by CPU
		fi

		dmesg | tail | grep "kvm: disabled by bios" > /dev/null
		if [ $? -eq 0 ]; then
			info "kvm disabled by bios. You can enable VT in bios"
			ret=
		fi

		lsmod | egrep '(kvm_intel|kvm_amd)' > /dev/null
		if [ $? -ne 0 ]; then
			info no kvm kernel module loaded
			ret=
		fi
	fi
	echo $ret;
}
get_user_undef_guess AUTOQEMU_KVM_ARG guess_kvm

guess_load_arg() {
	if [ $AUTOQEMU_ARCH != "arm" ]; then
		echo "-kernel $KERNEL"
	else
		./scripts/uboot-uimage >/dev/null
		if [ 0 -ne $? ]; then
			exit 1
		fi
		./scripts/qemu/beagle/run.sh beagle_nand.img ./uImage >/dev/null 2>/dev/null
		echo "-mtdblock beagle_nand.img"
	fi
}
get_user_empty_guess AUTOQEMU_LOAD_ARG guess_load_arg

guess_nographic() { echo "-nographic"; }
get_user_undef_guess AUTOQEMU_NOGRAPHIC_ARG guess_nographic

declare -A ARCH2QEMU
ARCH2QEMU=(
	[arm]="qemu-system-arm -M overo"
	[microblaze]="qemu-system-microblaze -M petalogix-s3adsp1800"
	[mips]="qemu-system-mips -M mipssim"
	[ppc]="qemu-system-ppc -M virtex-ml507"
	[sparc]="qemu-system-sparc -M leon3_generic -cpu LEON3"
	[x86]="qemu-system-i386"
)

declare -A NIC2QEMU
NIC2QEMU=(
	[e1000]="e1000"
	[ne2k_pci]="ne2k_pci"
	[rtl8139]="rtl8139"
	[virtio]="virtio"
	[xemaclite]="xilinx-ethlite:xlnx.xps-ethernetlite"
	[lan9118]="lan9118"
)

qemu_nics=$(echo $AUTOQEMU_NICS ${!NIC2QEMU[@]} | sed 's/\ \+/\n/g' | sort | uniq -d)

qemu_start_script=${AUTOQEMU_START_SCRIPT:-$DATADIR/start_script}
qemu_stop_script=${AUTOQEMU_STOP_SCRIPT:-$DATADIR/stop_script}
sudo=""
nic_lines=""
if [ -n "$qemu_nics" ]; then
	sudo=sudo
	nic_n=0
	for n in $qemu_nics; do
		nic_lines="$nic_lines -net nic,model=$n,macaddr=AA:BB:CC:DD:EE:0$(($nic_n + 2)) \
			-net tap,ifname=tap$nic_n,script=$qemu_start_script,downscript=$qemu_stop_script"
		nic_n=$(($nic_n + 1))
	done
fi

FS_TYPE=ext2
HDD_IMAGE="qemu_ext2_image.img"

mkdir qemu_tmp_dir
./scripts/continuous/fs/img-manage.sh $FS_TYPE $HDD_IMAGE build "qemu_tmp_dir"
rmdir qemu_tmp_dir
ARG_LINE="$sudo $AUTOQEMU_PREFIX${ARCH2QEMU[$AUTOQEMU_ARCH]} $AUTOQEMU_LOAD_ARG $AUTOQEMU_KVM_ARG -m $AUTOQEMU_MEM $nic_lines ${AUTOQEMU_NOGRAPHIC_ARG--nographic} -hda $HDD_IMAGE $@"

info "$ARG_LINE"
$ARG_LINE

