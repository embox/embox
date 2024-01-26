There are features of OS loading in Raspberry Pi:

stage 1: on-chip RAM is running, loading stage2 into L2 cache
stage 2: bootcode.bin is running. SDRAM enabled and loading stage 3
stage 3: loader.bin is running. Start.elf is loaded, go to stage 4
stage 4: start.elf does load the OS image from the file specified in the line

	kernel=<filename>

of the file config.txt. All the necessary files can be obtained from any Linux build for Raspberry PI,
for example from an assembly using buildroot. Create a fat16 partition on the SD-card
(with boot and lba labels) and copy the files there:

	bootcode.bin
	loader.bin
	start.elf
	fixup.dat
	config.txt

In config.txt adding lines:

	enable_uart=1
	kernel=u-boot-nodtb.bin.

Build u-boot with rpi_2_defconfig and copy the u-boot-not.bin file to the same partition of the SD card.
Build the embox and copy the embox.bin to the same partition of the SD card.
To autostart embox, create the boot.cmd file with the following contents:

	load mmc 0:1 0x8000 embox.bin
	go 0x8000

and build boot.scr

	mkimage -C none -A arm -T script -d boot.cmd boot.scr

Copy boot.scr to the same partition of the SD card.
Now SD card is ready to use on the Raspberry Pi model B board.

The other way is to use the utility genimage (https://github.com/pengutronix/genimage).
Create a folder and copy the files there:

	bootcode.bin
	loader.bin
	start.elf
	fixup.dat
	config.txt
	u-boot-nodtb.bin
	embox.bin
	boot.cmd

Run the script:

    sh ./create_image.sh

The sdcard.img file will be created. Now everything is ready to restore the SD card from image
and launch embox on the Raspberry Pi model B board
