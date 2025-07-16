Refer to platform/licheepi/templates/licheepi_4a/README.md.

In previous single core case, we use `go` command for booting up embox image. But in this smp case,
we cannot use that unfotunately. The reason is that `go` command only works for the BSP, APs still
not boot up.

For u-boot normal case, the APs spinning in the `secondary_hart_loop` and  waiting for `bootx`
cmd, BUT th1520 vender uboot does some $hity modification. we cannot boot APs even use `bootx`
cmd. And the mainline uboot is under development, so we cannot use that either.

So I did the patch for the vendor uboot to make it work temporarily.
The vendor code can work and pretty messy, so the patch uses an ugly way.

Here is the steps:
1. Download vendor u-boot source [1].
2. Apply the patch using `git am smp-uboot.patch` command under vender u-boot dir.
3. Build and burn the u-boot following [2]
4. Use command `tftp 0x20000000 embox;` and `bootelf 0x20000000` to boot up embox. (assume you set up the ip address and have tftp server running on your host)

Link: https://github.com/revyos/thead-u-boot.git [1]
Link: https://wiki.sipeed.com/hardware/en/lichee/th1520/lpi4a/4_burn_image.html#Linux [2]
