# PocketBeagle 2

PocketBeagle 2 support is provided by the
`platform/beagleboard/pocketbeagle2` template.

## Build

Use a bare-metal AArch64 toolchain:

```bash
export PATH=/opt/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf/bin:$PATH
make confload-platform/beagleboard/pocketbeagle2
make
aarch64-none-elf-objcopy -O binary build/base/bin/embox embox.bin
```

## Boot

U-Boot can load and run `embox.bin` from the FAT boot partition:

```text
setenv loadaddr 0x82000000
fatload mmc 1:1 ${loadaddr} embox.bin
go ${loadaddr}
```

PocketBeagle 2 uses `0x82000000` because U-Boot reserves the lower DDR range
and rejects raw loads at `0x80080000`.

If the board boots through `uEnv.txt`, place the following file next to
`embox.bin`:

```text
loadaddr=0x82000000
uenvcmd=fatload mmc 1:1 ${loadaddr} embox.bin; go ${loadaddr}
```
