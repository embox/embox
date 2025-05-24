#!/bin/sh

if ! command -v dtc >/dev/null 2>&1; then
  echo "Error: dtc not found !"
  exit 1
fi

if ! command -v mkfs.vfat >/dev/null 2>&1; then
  echo "Error: mkfs.vfat not found !"
  exit 1
fi

EMBOX_ELF="build/base/bin/embox"
EMBOX_BIN="build/base/bin/embox.bin"
ISONAME="embox.img"
MOUNT_DIR="embox-iso-tmp"
DEVICE_TREE="board_config/dts/dummy"
UBOOT_ARGS=()

help_info() {
	echo "Usage: $0 [options] [positional_args...]"
	echo
	echo "Options:"
	echo "  -o, --output <file>     Specify the output uImage file name"
	echo "  -i, --iso <name>        Specify the iso image name"
	echo "  -b, --bin <bin>         Specify the binary name"
	echo "  -e, --elf <elf>         Specify the elf name"
	echo "  -d, --dir <dir>         Specify the tmp mount dir name"
	echo "  -h, --help              Show this help message and exit"
	echo "  --u-boot <args>         All following args are passed to uboot-uimage script"
	echo
	echo "Example:"
	echo "  $0 -i embox.img -d embox-iso-tmp --u-boot -o uImage -a riscv"
	echo
}

while [[ $# -gt 0 ]]; do
	case $1 in
		--u-boot)
			shift
			while [[ $# -gt 0 && $1 != --* ]]; do
			UBOOT_ARGS+=("$1")
			shift
			done
			;;
		-i|--iso)
			ISONAME="$2"
			shift
			shift
			;;
		-d|--dir)
			MOUNT_DIR="$2"
			shift
			shift
			;;
		-h|--help)
			help_info
			exit 0
			;;
		*)
			echo "Unknown option: $1"
			help_info
			exit 1
			;;
	esac
done

echo "ISO           = ${ISONAME}"
echo "DTB           = ${DEVICE_TREE}.dtb"

UIMAGE_VALUE=$(./scripts/uboot-uimage ${UBOOT_ARGS[@]} | tee >(cat >&2) |  grep '^UIMAGE' | awk -F'= ' '{print $2}')

mkdir ${MOUNT_DIR} || { echo "${MOUNT_DIR} directory exist !!"; exit 1; }
dd if=/dev/zero of=${ISONAME} bs=1M count=64
mkfs.vfat -F 32 ${ISONAME}
dtc -I dts -O dtb -o $DEVICE_TREE.dtb $DEVICE_TREE.dts

mount -o loop ${ISONAME} ${MOUNT_DIR}
cp ${EMBOX_ELF} ${MOUNT_DIR}
cp ${EMBOX_BIN} ${MOUNT_DIR}
cp ${UIMAGE_VALUE} ${MOUNT_DIR}
cp $DEVICE_TREE.dtb ${MOUNT_DIR}
chmod 664 ${ISONAME}

umount ${MOUNT_DIR}
rmdir ${MOUNT_DIR}
