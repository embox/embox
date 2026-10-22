# ESP32-C5: standalone SPI flash startup

Build, flash and debug instructions: [RUN.md](RUN.md).

The loader and Embox are linked independently:

| Artifact | Purpose | Flash offset |
| --- | --- | --- |
| `bootloader.elf` / `bootloader.bin` | SRAM bootstrap, ESP image header | `0x2000` |
| `embox` / `embox.xip.bin` | Embox ELF / raw XIP payload | `0x10000` |

The ROM loads the standalone loader into SRAM. The loader disables the boot
watchdogs (LP and TG0 flash-boot), initializes the cache through ROM functions,
and maps a fixed 1 MiB
window of physical flash at `0x10000` to CPU address `0x42000000`. It then jumps
to `0x42000000`, where the application must place `_start`. The application
image tool validates that entry point and the ELF load segments. No ESP-IDF
partition table or application image header is used. The loader has no link-time
dependency on the application; both must follow this fixed address contract.

## SRAM ownership

Embox owns the full HP SRAM range `0x40800000`–`0x40860000` (384 KiB) after
handoff. Before handoff, the loader uses `0x40840000`–`0x40848000` for code and
constants, `0x40848000`–`0x40849000` for its temporary stack, and retains the
upper ROM working memory. The application is not loaded into SRAM by the loader.

The existing RISC-V `_start` executes from mapped flash, installs its own global
pointer, trap vector and stack, and only then clears BSS and copies `.data`
initializers. Those operations may overwrite the old loader and ROM workspace.
There is no return to the loader and no ROM service may be called after handoff.
Future ROM-dependent services, wireless stacks or other RAM users require their
own reservations before enabling them. LP RAM is not included in the 384 KiB.

## Build

Use an RV32-capable RISC-V toolchain and esptool with ESP32-C5 support:

```sh
make build-platform/espressif/esp32c5 CROSS_COMPILE=riscv32-esp-elf- -j4
make -f platform/espressif/esp32c5/boot/Makefile CROSS_COMPILE=riscv32-esp-elf-
python3 scripts/espressif/esp32c5_image.py \
    build/base/bin/embox build/base/bin/embox.xip.bin
```

The standalone makefile accepts `OUTPUT_DIR` (default `build/base/bin`) and
`ESPTOOL` (default `esptool`). Its flash settings are DIO/20 MHz and a 2 MiB
size field. The mapped application window ends at physical flash `0x110000`.
The ordinary `embox.bin` is not an ESP ROM boot image; use the validated XIP
binary and its separately built loader.

For an unencrypted development board, write both images through the UART port:

```sh
esptool --chip esp32c5 --port /dev/ttyUSB0 write-flash \
    0x2000 build/base/bin/bootloader.bin \
    0x10000 build/base/bin/embox.xip.bin
```

This replaces the existing bootloader and any partitions overlapped by either
image. The layout does not implement secure boot, flash encryption or OTA.
Once installed, the loader need not be rewritten for compatible applications.

For JTAG, use Espressif OpenOCD with `board/esp32c5-builtin.cfg`. Use `embox`
for application symbols and `bootloader.elf` for loader symbols. Plain GDB `load`
is not a substitute for writing the two flash images.

Expected early messages through the ROM-configured console:

```text
Embox ESP32-C5: SRAM boot
Embox ESP32-C5: flash mapped, entering _start
```

The earlier combined-image layout was verified on ESP32-C5-WIFI6-KIT-NXRX,
revision v1.0. The standalone handoff and reclaimed upper SRAM require a new
hardware run; host builds and image checks do not verify those behaviors.

Host-side image layout checks:

```sh
python3 -B scripts/espressif/test_esp32c5_image.py
```
