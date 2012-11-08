/**
 * @file
 *
 * @date 08.11.12
 * @author Ilia Vaprol
 */

/**
 * Exception table:
 *   0x0000 - Reserved
 *   0x0100 - System reset
 *   0x0200 - Machine check
 *   0x0300 - DSI
 *   0x0400 - ISI
 *   0x0500 - External interrupt
 *   0x0600 - Alignment
 *   0x0700 - Program
 *   0x0800 - Floating-point unavailable
 *   0x0900 - Decrementer
 *   0x0A00 - Reserved
 *   0x0B00 - Reserved
 *   0x0C00 - System call
 *   0x0D00 - Trace
 *   0x0E00 - Floating-point assist
 */

.section .trap_table.text

.align 8 /* at 0x0000 */
nop

.align 8 /* at 0x0100 */
sys_rst_hnd:
	b die

.align 8 /* at 0x0200 */
m_chk_hnd:
	b die

.align 8 /* at 0x0300 */
dsi_hnd:
	b die

.align 8 /* at 0x0400 */
isi_hnd:
	b die

.align 8 /* at 0x0500 */
ext_intr_hnd:
	b die

.align 8 /* at 0x0600 */
align_hnd:
	b die

.align 8 /* at 0x0700 */
prog_hnd:
	b die

.align 8 /* at 0x0800 */
fp_unavail_hnd:
	b die

.align 8 /* at 0x0900 */
decr_hnd:
	b die

.align 8 /* at 0x0A00 */
nop

.align 8 /* at 0x0B00 */
nop

.align 8 /* at 0x0C00 */
sys_call_hnd:
	b die

.align 8 /* at 0x0D00 */
trace_hnd:
	b die

.align 8 /* at 0x0E00 */
fp_assist_hnd:
	b die

die:
	b die
