/* ESP32-C5 ROM loads this section into SRAM before entering here. */

.section .text.boot_entry, "ax"
.balign 4
.global esp32c5_boot_entry
.type esp32c5_boot_entry, @function
esp32c5_boot_entry:
	csrci mstatus, 8

	/* Standalone C code is built without small-data addressing/relaxation. */
	la sp, __esp32c5_boot_stack_top

	/* An early fault must not enter the kernel with uninitialized data. */
	la t0, esp32c5_boot_trap
	csrw mtvec, t0
	tail esp32c5_boot
.size esp32c5_boot_entry, . - esp32c5_boot_entry

.balign 64
esp32c5_boot_trap:
	wfi
	j esp32c5_boot_trap
