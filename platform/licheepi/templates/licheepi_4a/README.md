Refer to platform/banana_pi/templates/bpi_p2_zero/README.md.

Making the environment for licheepi 4A to run:

- flash the embox image to fix address after BROM running out.(Not recommended)
- flash the embox to mmc storage(SD card or eMMC) using the uboot to load up.(For stable usage)
- load the embox image from uboot on the fly, eg..tftp, fastboot (Recommended for testing).
	for example tftp:
	```
	tftp 0x0 embox.bin
	go 0x0
	```

Uboot from vendor can be down load from [1]. More infomations can be found in wiki [2].

Link: https://wiki.sipeed.com/hardware/en/lichee/th1520/lpi4a/3_images.html [1]
Link: https://wiki.sipeed.com/hardware/en/lichee/th1520/lpi4a/1_intro.html [2]
