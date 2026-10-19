# ESP32-C5: experimental SPI flash startup

Build, flash and debug instructions: [RUN.md](RUN.md).

This template is under bring-up. The interrupt, timer and UART modules still
come from ESP32-C3 and have not been ported to C5. A working shell is not yet
expected. Do not use the ordinary `embox.bin` as an ESP boot image.

The ROM loads `.esp32c5_boot` into SRAM from an ESP image at flash offset
`0x2000`. The bootstrap retains the ROM-selected SPI clock/mode, disables the
boot watchdog, initializes the cache through ROM functions and maps physical
flash `0x10000` at CPU address `0x42000000`. It then enters the existing RISC-V
`_start`. That entry clears BSS and copies `.data` initializers from mapped
flash to SRAM. No ESP-IDF partition table or separate bootloader is used.

Memory reservations:

| Purpose | Address range (end excluded) |
| --- | --- |
| Embox RAM | `0x40800000`–`0x40840000` |
| Bootstrap and temporary stack | `0x40840000`–`0x40849000` |
| Upper SRAM, reserved for ROM | `0x40849000`–`0x40860000` |
| Embox mapped flash window | `0x42000000`–`0x42100000` |

Build using an RV32-capable RISC-V toolchain. With the Espressif toolchain,
override `CROSS_COMPILE=riscv32-esp-elf-` when invoking make.

```sh
make build-platform/espressif/esp32c5 CROSS_COMPILE=riscv32-esp-elf- -j4
```

After building, pack the ELF (not the raw binary):

```sh
python3 scripts/espressif/esp32c5_image.py \
    build/base/bin/embox build/base/bin/embox.flash.bin \
    --objcopy riscv32-esp-elf-objcopy
```

The packer requires `esptool` with ESP32-C5 support. `--esptool` accepts its
executable path. It creates a normal ESP RAM image using esptool and appends
the XIP payload at a fixed flash offset. The header deliberately advertises
only the RAM bootstrap. It uses conservative DIO/20 MHz and a 2 MiB size field;
only the first 0x110000 bytes of physical flash are used at most.

For an unencrypted development board, write the resulting image at `0x2000`
through the UART connector:

```sh
esptool --chip esp32c5 --port /dev/ttyUSB0 write-flash \
    0x2000 build/base/bin/embox.flash.bin
```

This replaces the existing bootloader and any partitions overlapped by the
image. The layout does not implement secure boot, flash encryption or OTA.

For JTAG, use Espressif OpenOCD with `board/esp32c5-builtin.cfg`. Load symbols
from the full `embox` ELF. The first useful hardware breakpoints are
`esp32c5_boot_entry`, `_start` and `kernel_start`. Plain GDB `load` of this ELF
is not a substitute for packing and writing the flash image.

Expected early messages, through the ROM-configured console:

```text
Embox ESP32-C5: SRAM boot
Embox ESP32-C5: flash mapped, entering _start
```

Boot to `_start` and `kernel_start`, and UART diagnostic output have been
verified on ESP32-C5-WIFI6-KIT-NXRX with chip revision v1.0. Interrupts,
timer ticks and UART RX remain unverified; `.data` contents have not been
checked separately.

Host-side image layout checks:

```sh
python3 -B scripts/espressif/test_esp32c5_image.py
```
