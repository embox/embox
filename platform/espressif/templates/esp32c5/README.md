# Embox on ESP32-C5

This guide covers environment setup, building Embox, and flashing an ESP32-C5
board through either GDB with OpenOCD or a serial port with esptool.
The connection examples use a board with separate USB JTAG and UART connectors.

Run the build and flashing commands from the Embox repository root.

## 1. Prepare the environment

### General tools

Install Git, GNU Make, a native C/C++ build toolchain, Python 3 with pip and
venv support, and a bare-metal RISC-V GCC toolchain with Binutils.
The cross compiler must support RV32 with the ILP32 ABI and the
`rv32imc_zicsr_zifencei` instruction set. A compiler named
`riscv64-unknown-elf-gcc` can build RV32 code when its multilib configuration
supports this target.

The commands below use the template's default prefix,
`riscv64-unknown-elf-`. Add the toolchain's `bin` directory to `PATH` and check:

```sh
riscv64-unknown-elf-gcc --version
riscv64-unknown-elf-gcc -print-multi-lib
riscv64-unknown-elf-objcopy --version
```

For the GDB flashing method, install GNU GDB with RISC-V support. The examples
use `gdb-multiarch`; a RISC-V bare-metal GDB can also be used.
For console access, install a serial terminal such as minicom.

### ESP-specific tools

ESP-IDF is not required. The two ESP-specific tools used here are esptool and
Espressif's OpenOCD distribution.

Install esptool in a Python virtual environment outside the source tree:

```sh
python3 -m venv ~/.venvs/embox-esp32c5
. ~/.venvs/embox-esp32c5/bin/activate
python3 -m pip install 'esptool==5.3.1'
esptool --chip esp32c5 --help
```

Activate this environment in each terminal that builds the bootloader or uses
esptool. Version 5.3.1 is the version recorded in the existing board notes.
See the [esptool installation documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32/installation.html)
for installation details.

For JTAG flashing, download an archive for your host from the
[Espressif OpenOCD releases](https://github.com/espressif/openocd-esp32/releases).
Choose a release containing `board/esp32c5-builtin.cfg`. Extract it, then add
its executable and configuration directories to the environment, replacing
`/path/to/openocd-esp32` with the extracted directory:

```sh
export PATH="/path/to/openocd-esp32/bin:$PATH"
export OPENOCD_SCRIPTS="/path/to/openocd-esp32/share/openocd/scripts"
openocd --version
```

Use the executable and scripts from the same distribution. On Linux, install
the supplied `contrib/60-openocd.rules` as a system udev rule if USB access
requires it, reload the rules, and reconnect the board. Your user also needs
permission to access the serial device, typically through the `dialout` or
`uucp` group, depending on the distribution.

The [ESP32-C5 JTAG documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c5/api-guides/jtag-debugging/index.html)
describes the built-in USB JTAG connection and OpenOCD configuration.

## 2. Build the images

Load the ESP32-C5 template and build the application:

```sh
make confload-platform/espressif/esp32c5
make -j"$(nproc)" CROSS_COMPILE=riscv64-unknown-elf-
```

Loading a template replaces the active configuration in `conf/`.
If your toolchain uses another prefix, pass that prefix as `CROSS_COMPILE`
to both the application build and the bootloader build below.

Build the standalone bootloader, then generate the application's XIP image:

```sh
make -f platform/espressif/esp32c5/boot/Makefile \
    CROSS_COMPILE=riscv64-unknown-elf-
python3 scripts/espressif/esp32c5_image.py \
    build/base/bin/embox build/base/bin/embox.xip.bin
esptool --chip esp32c5 image-info build/base/bin/bootloader.bin
```

The image generator checks the application's ELF layout and requires an entry
point at `0x42000000`. The bootloader image should contain one RAM segment at
`0x40840000`, with its entry point inside that segment and valid checksum/hash.

| File | Purpose | Flash offset |
| --- | --- | --- |
| `build/base/bin/bootloader.bin` | Standalone SRAM bootloader | `0x2000` |
| `build/base/bin/embox.xip.bin` | Application executed from mapped flash | `0x10000` |
| `build/base/bin/embox` | Application ELF with GDB symbols | — |
| `build/base/bin/bootloader.elf` | Bootloader ELF with GDB symbols | — |

Use the two `.bin` files listed above for either flashing method. The ordinary
`embox.bin` is not suitable for this boot layout. No ESP-IDF partition table
is used. Flashing replaces the existing bootloader and any data overlapped by
the application image.

## 3. Flash the board

Choose one of the following methods. Both write the same images at the same
flash offsets.

### Option A: GDB with OpenOCD over JTAG

Connect the board's built-in USB JTAG connector, labelled **USB** on the board
used in the existing notes. Start OpenOCD and leave it running:

```sh
openocd -f board/esp32c5-builtin.cfg
```

Wait for the GDB server to listen on port `3333`. In another terminal, from
the repository root, open the application ELF:

```sh
gdb-multiarch build/base/bin/embox
```

Run these commands in GDB. Replace `/absolute/path/to/embox` with the absolute
repository path. OpenOCD opens the binary files, so absolute paths avoid
depending on its working directory.

```gdb
target extended-remote localhost:3333
monitor reset halt
monitor program_esp /absolute/path/to/embox/build/base/bin/bootloader.bin 0x2000 verify
monitor program_esp /absolute/path/to/embox/build/base/bin/embox.xip.bin 0x10000 verify
```

After both writes report successful verification, reset and stop at the
application entry point:

```gdb
monitor reset halt
hbreak _start
continue
```

The expected entry address is `0x42000000`. To run Embox from there:

```gdb
delete breakpoints
continue
```

Use the ELF from the same build as the flashed application. Flash writes are
performed by OpenOCD's `program_esp` command; GDB's `load` command does not
install this two-image flash layout.

### Option B: esptool over serial

Connect the board's **UART** connector and identify its serial device.
```sh
esptool --chip esp32c5 --port /dev/ttyACM0 write-flash \
    0x2000 build/base/bin/bootloader.bin \
    0x10000 build/base/bin/embox.xip.bin
```

If automatic download-mode entry fails, hold **BOOT**, press and release
**RESET**, then release **BOOT** and retry. After flashing, release **BOOT**
and press **RESET** if the application does not start automatically.

## 4. Connect to Embox shell

Open the UART device with a serial terminal:

```sh
minicom -D /dev/ttyACM0 -b 115200
```

