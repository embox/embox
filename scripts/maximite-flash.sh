##
# @file
# @brief Performs flashing of Maximite via it's bootloader (HID)
#        Requires cross-compile's objcopy, sed and
#	 mphidflash (http://code.google.com/p/mphidflash/)
# @author  Anton Kozlov
# @date    18.09.2012
#

#assuming we are running from embox root, scripts/$0 for example
BUILD_DIR=./build/base/bin
CROSS_COMPILE=mips-elf

# below listed vid and pid of Olimex Duinomite (Maximite compatible clone)
MAXIMITE_VID=0x15ba
MAXIMITE_PID=0x32

EM_SREC=$BUILD_DIR/embox.srec
EM_HEX=.embox.hex


# obtaining hex from elf would fail with objcopy error, obtaining from
# srec instead
$CROSS_COMPILE-objcopy -I srec -O ihex $EM_SREC $EM_HEX

# mphidflash does'nt support 'set pc' derective, cleaning out it from hex
sed -i 's/:04000005.*//' $EM_HEX

sudo mphidflash -n -v $MAXIMITE_VID -p $MAXIMITE_PID -w $EM_HEX

rm $EM_HEX

