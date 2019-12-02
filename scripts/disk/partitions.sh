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
PARTS=$2 
FS=$3
SIZE=$4

[ "$PARTS" == "" ] && PARTS=4
[ "$FS" == "" ] && FS="fat"
[ "$SIZE" == "" ] && SIZE=128

if [[ ! $PARTS = 1 && ! $PARTS = 2 && ! $PARTS = 3 && ! $PARTS = 4 ]]; then
	echo "MBR may have up to 4 partitions!"
	exit 1
fi

MKFS_PATH=`sudo which mkfs.$FS 2> /dev/null`
[ "$MKFS_PATH" == "" ] && echo "Command mkfs.$FS not found" && exit

dd if=/dev/zero of=$IMG count=$SIZE bs=1M
sudo parted $IMG -s mktable msdos || exit

for i in $(seq 1 $PARTS)
do
	sudo parted $IMG -s mkpart primary fat32 \
		$(( $i * $SIZE / ($PARTS + 1) )) $(( (1 + $i) * $SIZE / ($PARTS + 1) ))
done

sudo kpartx -d $IMG || exit

LOOPDEV=$(sudo kpartx -av $IMG | head -n 1 | awk "{ print \$3"})
LOOPDEV=${LOOPDEV::-1}

for i in $(seq 1 $PARTS)
do
	sudo mkfs.$FS -n "PART$i" -I /dev/mapper/$LOOPDEV$i
	sudo mount /dev/mapper/$LOOPDEV$i /mnt
	sudo bash -c "echo Partition $i file >> /mnt/part$i\_file"
	sudo umount /mnt
	sync
done
