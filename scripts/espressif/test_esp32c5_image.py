"""Host-side checks for the C5 flash layout; no board or toolchain needed."""

import struct
import unittest

from esp32c5_image import flash_payload


def executable(segments, entry=0x40840000):
    """Construct ELF32 PT_LOAD records from (VMA, LMA, bytes)."""
    ident = b"\x7fELF\x01\x01\x01" + bytes(9)
    image = bytearray(ident + struct.pack(
        "<HHIIIIIHHHHHH", 2, 243, 1, entry, 52, 0, 0,
        52, 32, len(segments), 0, 0, 0))
    offset = 52 + 32 * len(segments)
    for vaddr, paddr, data in segments:
        image.extend(struct.pack("<IIIIIIII", 1, offset, vaddr, paddr,
                                 len(data), len(data), 5, 4))
        offset += len(data)
    for _, _, data in segments:
        image.extend(data)
    return image


class FlashLayoutTest(unittest.TestCase):
    def test_data_initializers_use_flash_lma(self):
        image = executable([
            (0x40840000, 0x40840000, b"boot"),
            (0x42000000, 0x42000000, b"code"),
            (0x40808000, 0x42000010, b"data"),
        ])
        self.assertEqual(flash_payload(image),
                         b"code" + b"\xff" * 12 + b"data")

    def test_rom_entry_cannot_point_into_unmapped_flash(self):
        image = executable([(0x42000000, 0x42000000, b"code")],
                           entry=0x42000000)
        with self.assertRaisesRegex(ValueError, "entry"):
            flash_payload(image)

    def test_boot_cannot_overwrite_temporary_stack(self):
        image = executable([
            (0x40847ffc, 0x40847ffc, b"overflow"),
            (0x42000000, 0x42000000, b"code"),
        ])
        with self.assertRaisesRegex(ValueError, "unsupported load"):
            flash_payload(image)

    def test_overlapping_flash_segments_are_rejected(self):
        image = executable([
            (0x40840000, 0x40840000, b"boot"),
            (0x42000000, 0x42000000, b"code"),
            (0x40808000, 0x42000002, b"data"),
        ])
        with self.assertRaisesRegex(ValueError, "overlapping"):
            flash_payload(image)


if __name__ == "__main__":
    unittest.main()
