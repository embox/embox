# BeaglePlay

BeaglePlay support is provided by the `platform/beagleboard/beagleplay`
template.

## Build

Use a bare-metal AArch64 toolchain:

```bash
export PATH=/opt/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf/bin:$PATH
make confload-platform/beagleboard/beagleplay
make
aarch64-none-elf-objcopy -O binary build/base/bin/embox embox.bin
```

## Boot

U-Boot can load and run `embox.bin` from the FAT boot partition:

```text
setenv loadaddr 0x80080000
fatload mmc 0:1 ${loadaddr} embox.bin
go ${loadaddr}
```

The template uses UART0 at `0x02800000` with GIC interrupt 178.
