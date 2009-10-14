#!/bin/sh

# GRMON
GRMON_EVAL=grmon-eval
GRMON_EVAL_ARCH=$GRMON_EVAL-1.1.35b.tar.gz
GAISLER_GRMON_URL=ftp://gaisler.com/gaisler.com/grmon/$GRMON_EVAL_ARCH
BACH_FILE=cmd.grmon

# Image
BIN_DIR=bin
MONITOR_ROM=monitor_rom
SIZE_ROM=0x80000
MONITOR_RAM=monitor_ram
AUTOCONF=.config
AUTOCONF2=.config2

# Login
USER=sikmir
HOST=10.0.3.97
IMG_PATH=/home/$USER
DSA_KEY_FILE=~/.ssh/id2_dsa

if [ ! -e $AUTOCONF -a ! -e $AUTOCONF2 ]; then
    make xconfig
else
    make clean
    make
    if [ -e $BIN_DIR/$MONITOR_ROM ]; then
	if [ ! -e $GRMON_EVAL ]; then
	    if [ ! -e $GRMON_EVAL_ARCH ]; then
		echo "Download grmon-eval"
		wget -nv $GAISLER_GRMON_URL
	    fi
	    if [ -e $GRMON_EVAL_ARCH ]; then
		tar -xf $GRMON_EVAL_ARCH
		mv $GRMON_EVAL tmp
		cp tmp/linux/$GRMON_EVAL $GRMON_EVAL
		rm -fr tmp $GRMON_EVAL_ARCH
	    else
		echo "gaisler.com is unreachible"
		exit 0
	    fi
	fi
	cp $BIN_DIR/$MONITOR_ROM $MONITOR_ROM
	echo "flash unlock all" > $BACH_FILE
	echo "flash erase 0 "$SIZE_ROM >> $BACH_FILE
	echo "flash load "$MONITOR_ROM >> $BACH_FILE
	echo "exit" >> $BACH_FILE
	$GRMON_EVAL -nb -xilusb -c $BACH_FILE
	rm -f $MONITOR_ROM $BACH_FILE
	if [ -e $DSA_KEY_FILE ]; then
	    scp -i $DSA_KEY_FILE $BIN_DIR/$MONITOR_ROM $USER@$HOST:$IMG_PATH
	fi
    fi
    if [ -e $BIN_DIR/$MONITOR_RAM ]; then
	if [ -e $DSA_KEY_FILE ]; then
	    scp -i $DSA_KEY_FILE $BIN_DIR/$MONITOR_RAM $USER@$HOST:$IMG_PATH
	fi
    fi
    svn diff > diff.log
fi
