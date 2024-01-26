After clone u-boot ((https://github.com/u-boot/u-boot.git)) copy configs/bananapi_m2_zero_defconfig
to the file configs/bananapi_p2_zero_defconfig and add lines (https://linux-sunxi.org/Sinovoip_Banana_Pi_P2_Zero):

	CONFIG_MMC_SUNXI_SLOT_EXTRA=2
	CONFIG_SUN8I_EMAC=y
	CONFIG_USB_EHCI_HCD=y

make u-boot

	make bananapi_p2_zero_defconfig
	make;make all

Create the file boot.cmd with the following contents:

	load mmc 0:1 0x43008000 /boot/embox.bin
	go 0x43008000

and  build boot.scr:

	mkimage -C none -A arm -T script -d boot.cmd boot.scr

After build embox insert SD-card and run the script:

	sudo sh ./bpi_p2_sdc.sh

Now SD-card is ready to use on the Banana Pi P2 Zero board

The other way is to use the utility genimage (https://github.com/pengutronix/genimage).
Create a folder and copy the files there:

	boot.cmd
	embox.bin
	genimage.cfg
	u-boot-sunxi-with-spl.bin

Run the script:

    sh ./create_image.sh

The sdcard.img file will be created. Now everything is ready to restore the SD card and
launch embox on the Banana Pi P2 Zero board


TODO:
	* append support mmc file system
	* append support networking
	* append support eMMC on board
