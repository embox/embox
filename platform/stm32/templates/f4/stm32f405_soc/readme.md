To run you need to enter the following commands

$ make confload-platform/stm32/f4/stm32f405_soc
$ make
$ qemu-system-arm -kernel build/base/bin/embox -M netduinoplus2 -m 256 -no-reboot -serial stdio

Debugging

1) In one console
$ qemu-system-arm -gdb tcp::1234 -kernel build/base/bin/embox -M netduinoplus2 -m 256 -no-reboot -serial stdio -S

2) In another console
$ gdb-multiarch ./build/base/bin/embox
