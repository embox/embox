#!/bin/bash

# Create image file with 4 partitions

print_usage() {
	echo "Create image file with MBR partitions"
	echo "Default size is 128MiB with 4 partitions formatted with fat"
	echo ""
	echo "Usage:"
	echo "    $0 <target_file> <partitions> <fs_type> <size_mb>"
}

[ $# = 0 ] && print_usage && exit

IMG=$1
PARTS=${2:-4}
FS=${3:-fat}
SIZE=${4:-128}

# Check that parent dir of $IMG exists and it's possible to create a new file in it
IMG_DIR=$(dirname "$IMG")
if ! [ -d "$IMG_DIR" ] || ! [ -w "$IMG_DIR" ]; then
    echo "The path $IMG_DIR does not exist or isn't writeable. Can't create file $IMG"
    exit 1
fi

# Check that the number of partitions is correct
if [[ $PARTS -lt 1 ]] || [[ $PARTS -gt 4 ]]; then
	echo "MBR may have up to 4 partitions!"
	exit 1
fi

# Check that $FS refers to known filesystem
if ! command -v "mkfs.$FS" >/dev/null; then
    echo "Command mkfs.$FS not found"
    exit 1
fi

dd if=/dev/zero "of=$IMG" "count=$SIZE" bs=1M
sudo parted "$IMG" -s mktable msdos || { echo "Failed to create a partition table in $IMG"; exit 1; }

PARTITION_SIZE=$(( SIZE / PARTS ))
OFFSET=0
for i in $(seq 1 $PARTS)
do
	# 'mkpart' subcommand  usage (from 'man parted'):
	#     mkpart [part-type name fs-type] start end

	sudo parted "$IMG" -s mkpart primary fat32 $OFFSET $(( OFFSET + PARTITION_SIZE ))
	OFFSET=$(( OFFSET + PARTITION_SIZE ))
done

sudo kpartx -d "$IMG" || { echo "Failed to delete mappings: 'kpartx -d $IMG' failed"; exit 1; }

# Output of kpartx look like this
#  add map loop0p1 (254:1): 0 62500 linear 7:0 1
#  add map loop0p2 (254:2): 0 62500 linear 7:0 62501
#  add map loop0p3 (254:3): 0 62500 linear 7:0 125001
#  add map loop0p4 (254:4): 0 61440 linear 7:0 188416
LOOPDEV=$(sudo kpartx -av "$IMG" | head -n 1 | awk "{ print \$3 }")
# this cuts off the last symbol
LOOPDEV=${LOOPDEV::-1}

for i in $(seq 1 $PARTS)
do
	sudo "mkfs.$FS" -n "PART$i" -I "/dev/mapper/$LOOPDEV$i"
	sudo mount "/dev/mapper/$LOOPDEV$i" /mnt
	sudo bash -c "echo Partition $i file >> /mnt/part$i\_file"
	sudo umount /mnt
	sync
done
