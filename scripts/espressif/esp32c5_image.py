#!/usr/bin/env python3
"""Pack the C5 SRAM bootstrap and fixed-address XIP payload into one image."""

import argparse
import os
from pathlib import Path
import struct
import subprocess
import tempfile


BOOT_OFFSET = 0x2000
XIP_OFFSET = 0x10000
XIP_BASE = 0x42000000
XIP_SIZE = 0x100000
BOOT_BASE = 0x40840000
BOOT_END = 0x40848000  # Leave 4 KiB for the temporary stack.


def flash_payload(elf):
    """Use ELF physical addresses: .data runs in SRAM but is stored in XIP."""
    if elf[:7] != b"\x7fELF\x01\x01\x01":
        raise ValueError("expected a little-endian ELF32 executable")
    header = struct.unpack_from("<HHIIIIIHHHHHH", elf, 16)
    kind, machine, _, entry, phoff = header[:5]
    phsize, phnum = header[8:10]
    if kind != 2 or machine != 243 or phsize != 32:
        raise ValueError("expected a RISC-V ELF32 executable")
    if not BOOT_BASE <= entry < BOOT_END:
        raise ValueError("ELF entry must be esp32c5_boot_entry in boot SRAM")

    chunks = []
    boot_entry_loaded = False
    for index in range(phnum):
        fields = struct.unpack_from("<IIIIIIII", elf, phoff + index * phsize)
        ptype, offset, vaddr, paddr, filesz, memsz, flags, _ = fields
        if ptype != 1 or not filesz:
            continue
        if offset + filesz > len(elf) or filesz > memsz:
            raise ValueError("invalid ELF load segment")
        if XIP_BASE <= paddr and paddr + filesz <= XIP_BASE + XIP_SIZE:
            chunks.append((paddr - XIP_BASE, elf[offset:offset + filesz]))
        elif (BOOT_BASE <= paddr == vaddr
              and paddr + memsz <= BOOT_END):
            if flags & 1 and paddr <= entry < paddr + filesz:
                boot_entry_loaded = True
        else:
            raise ValueError(f"unsupported load segment at {paddr:#x}")
    if not boot_entry_loaded:
        raise ValueError("entry is not backed by an executable boot segment")
    if not chunks or min(offset for offset, _ in chunks) != 0:
        raise ValueError("flash payload must start at 0x42000000")

    result = bytearray()
    for offset, data in sorted(chunks):
        if offset < len(result):
            raise ValueError("overlapping flash load segments")
        result.extend(b"\xff" * (offset - len(result)))
        result.extend(data)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--objcopy", default=os.environ.get(
        "CROSS_COMPILE", "riscv64-unknown-elf-") + "objcopy")
    parser.add_argument("--esptool", default="esptool")
    args = parser.parse_args()

    try:
        payload = flash_payload(args.elf.read_bytes())
        with tempfile.TemporaryDirectory(prefix="embox-c5-") as directory:
            ram_elf = Path(directory) / "boot.elf"
            ram_image = Path(directory) / "boot.bin"
            subprocess.run([args.objcopy, "--only-section=.esp32c5_boot",
                            str(args.elf), str(ram_elf)], check=True)
            subprocess.run([args.esptool, "--chip", "esp32c5", "elf2image",
                            "--flash-mode", "dio", "--flash-freq", "20m",
                            "--flash-size", "2MB", "-o", str(ram_image),
                            str(ram_elf)], check=True)
            boot = ram_image.read_bytes()
        padding = XIP_OFFSET - BOOT_OFFSET - len(boot)
        if padding < 0:
            raise ValueError("SRAM boot image overlaps the flash payload")
        args.output.write_bytes(boot + b"\xff" * padding + payload)
    except (ValueError, OSError, struct.error,
            subprocess.CalledProcessError) as error:
        parser.exit(1, f"ESP32-C5 image: {error}\n")
    print(f"Wrote {args.output}; flash at {BOOT_OFFSET:#x}")


if __name__ == "__main__":
    main()
