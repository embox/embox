#!/usr/bin/env python3
"""Create the standalone C5 application's XIP binary; flash it at 0x10000."""

import argparse
from pathlib import Path
import struct


XIP_OFFSET = 0x10000
XIP_BASE = 0x42000000
XIP_SIZE = 0x1600000
RAM_BASE = 0x40800000
RAM_END = 0x40860000


def flash_payload(elf):
    """Use ELF physical addresses: .data runs in SRAM but is stored in XIP."""
    if elf[:7] != b"\x7fELF\x01\x01\x01":
        raise ValueError("expected a little-endian ELF32 executable")
    header = struct.unpack_from("<HHIIIIIHHHHHH", elf, 16)
    kind, machine, _, entry, phoff = header[:5]
    phsize, phnum = header[8:10]
    if kind != 2 or machine != 243 or phsize != 32:
        raise ValueError("expected a RISC-V ELF32 executable")
    if entry != XIP_BASE:
        raise ValueError("ELF entry must be _start at 0x42000000")

    chunks = []
    entry_loaded = False
    for index in range(phnum):
        fields = struct.unpack_from("<IIIIIIII", elf, phoff + index * phsize)
        ptype, offset, vaddr, paddr, filesz, memsz, flags, _ = fields
        if ptype != 1:
            continue
        if offset + filesz > len(elf) or filesz > memsz:
            raise ValueError("invalid ELF load segment")
        if not memsz:
            continue
        in_ram = RAM_BASE <= vaddr and vaddr + memsz <= RAM_END
        in_flash = XIP_BASE <= vaddr and vaddr + memsz <= XIP_BASE + XIP_SIZE
        if not in_ram and not in_flash:
            raise ValueError(f"unsupported runtime segment at {vaddr:#x}")
        if not filesz:
            continue
        if not XIP_BASE <= paddr < paddr + filesz <= XIP_BASE + XIP_SIZE:
            raise ValueError(f"unsupported flash load segment at {paddr:#x}")
        if in_flash and (vaddr != paddr or memsz != filesz):
            raise ValueError("XIP segment must execute at its load address")
        chunks.append((paddr - XIP_BASE, elf[offset:offset + filesz]))
        if flags & 1 and vaddr == paddr == entry:
            entry_loaded = True
    if not entry_loaded:
        raise ValueError("entry is not backed by an executable XIP segment")

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
    args = parser.parse_args()

    try:
        args.output.write_bytes(flash_payload(args.elf.read_bytes()))
    except (ValueError, OSError, struct.error) as error:
        parser.exit(1, f"ESP32-C5 image: {error}\n")
    print(f"Wrote {args.output}; flash at {XIP_OFFSET:#x}")


if __name__ == "__main__":
    main()
