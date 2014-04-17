#!/bin/sh

NAND_IMG=$1
UIMAGE=$2

DATA_DIR=$(dirname $0)

rm $NAND_IMG

$DATA_DIR/bb_nandflash.sh $DATA_DIR/x-load.bin.ift $NAND_IMG x-loader
$DATA_DIR/bb_nandflash.sh $DATA_DIR/u-boot.bin $NAND_IMG u-boot
$DATA_DIR/bb_nandflash.sh $UIMAGE $NAND_IMG kernel
#$DATA_DIR/bb_nandflash.sh $DATA_DIR/rd-ext2-8M.bin $NAND_IMG  rootfs

make -C $DATA_DIR bb_nandflash_ecc
$DATA_DIR/bb_nandflash_ecc $NAND_IMG 0x0 0xe80000

# copying u_boot environment
dd if=$DATA_DIR/uboot_env.bin of=$NAND_IMG seek=1254 conv=notrunc bs=2048
