SECTIONS {
	/* It's from SDK: sdk/bsp/ldscripts/ble_projects/sections_da1469x.ld.h.
	 * See also REMAP_INTVECT bit in SYS_CTRL_REG register (DA1469x datasheet).
	 */
	.da1469x_quirk : {
		. += 0x200;
	}
}
