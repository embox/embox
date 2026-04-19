# TI AM62x

Initial Embox support for the Texas Instruments AM62x SoC family on:

- BeagleBoard.org PocketBeagle 2
- BeagleBoard.org BeaglePlay

Support is provided through the following templates:

| Template | Board | Console UART | Load Address |
| --- | --- | --- | --- |
| `aarch64/pocketbeagle2` | PocketBeagle 2 | UART6 (`0x02860000`, IRQ 216) | `0x82000000` |
| `aarch64/beagleplay` | BeaglePlay | UART0 (`0x02800000`, IRQ 178) | `0x80080000` |

Both templates use the generic `aarch64` Embox support together with:

- GICv3 at `0x01800000` / `0x01880000`
- ARMv8 physical timer on INTID 30
- `embox.driver.serial.ns16550` with `clk_freq=48000000`, `reg_width=4`,
  `mem32_access=true`

## Build

Use a bare-metal AArch64 toolchain:

```bash
export PATH=/opt/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf/bin:$PATH
```

Build for PocketBeagle 2:

```bash
make confload-aarch64/pocketbeagle2
make
```

Build for BeaglePlay:

```bash
make confload-aarch64/beagleplay
make
```

Convert the resulting ELF image to a flat binary for U-Boot:

```bash
aarch64-none-elf-objcopy -O binary build/base/bin/embox embox.bin
```

## Boot

BeaglePlay:

```text
setenv loadaddr 0x80080000
fatload mmc 0:1 ${loadaddr} embox.bin
go ${loadaddr}
```

PocketBeagle 2:

```text
setenv loadaddr 0x82000000
fatload mmc 1:1 ${loadaddr} embox.bin
go ${loadaddr}
```

PocketBeagle 2 uses `0x82000000` because U-Boot reserves the lower DDR range
and rejects raw loads at `0x80080000`.

## PocketBeagle 2 SD Card

Copy `embox.bin` to the FAT boot partition:

```bash
cp embox.bin /media/$USER/BOOT1/embox.bin
sync
```

If the board boots through `uEnv.txt`, place the following file next to
`embox.bin`:

```text
loadaddr=0x82000000
uenvcmd=fatload mmc 1:1 ${loadaddr} embox.bin; go ${loadaddr}
```

Adjust the MMC device or mount path if your U-Boot or host setup differs.
