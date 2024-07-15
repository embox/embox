/**
 * @file Clock
 * @author Ghulam Murtaza
 * @date 10.07.2024
 */

#include <util/log.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <embox/unit.h>
#include <drivers/common/memory.h>
#include <config/board_config.h>
#include <framework/mod/options.h>
#include <drivers/gpio/k210/fpioa.h>
#include <drivers/clk/maix_bit_sysctl.h>


// /* From board_config.h*/
// #define CLK_NAME_FPIOA

#define K210_SYSCTL_GIT_ID	    0x00 /* Git short commit id */
#define K210_SYSCTL_UART_BAUD	0x04 /* Default UARTHS baud rate */
#define K210_SYSCTL_PLL0	    0x08 /* PLL0 controller */
#define K210_SYSCTL_PLL1	    0x0C /* PLL1 controller */
#define K210_SYSCTL_PLL2	    0x10 /* PLL2 controller */
#define K210_SYSCTL_PLL_LOCK	0x18 /* PLL lock tester */
#define K210_SYSCTL_ROM_ERROR	0x1C /* AXI ROM detector */
#define K210_SYSCTL_SEL0	    0x20 /* Clock select controller 0 */
#define K210_SYSCTL_SEL1	    0x24 /* Clock select controller 1 */
#define K210_SYSCTL_EN_CENT	    0x28 /* Central clock enable */
#define K210_SYSCTL_EN_PERI	    0x2C /* Peripheral clock enable */
#define K210_SYSCTL_SOFT_RESET	0x30 /* Soft reset ctrl */
#define K210_SYSCTL_PERI_RESET	0x34 /* Peripheral reset controller */
#define K210_SYSCTL_THR0	    0x38 /* Clock threshold controller 0 */
#define K210_SYSCTL_THR1	    0x3C /* Clock threshold controller 1 */
#define K210_SYSCTL_THR2	    0x40 /* Clock threshold controller 2 */
#define K210_SYSCTL_THR3	    0x44 /* Clock threshold controller 3 */
#define K210_SYSCTL_THR4	    0x48 /* Clock threshold controller 4 */
#define K210_SYSCTL_THR5	    0x4C /* Clock threshold controller 5 */
#define K210_SYSCTL_THR6	    0x50 /* Clock threshold controller 6 */
#define K210_SYSCTL_MISC	    0x54 /* Miscellaneous controller */
#define K210_SYSCTL_PERI	    0x58 /* Peripheral controller */
#define K210_SYSCTL_SPI_SLEEP	0x5C /* SPI sleep controller */
#define K210_SYSCTL_RESET_STAT	0x60 /* Reset source status */
#define K210_SYSCTL_DMA_SEL0	0x64 /* DMA handshake selector 0 */
#define K210_SYSCTL_DMA_SEL1	0x68 /* DMA handshake selector 1 */
#define K210_SYSCTL_POWER_SEL	0x6C /* IO Power Mode Select controller */


EMBOX_UNIT_INIT(k210_fpioa_init);
// TODO: add sysctl driver

// central clock enable(sysctl: 0x28)
volatile sysctl_clock_enable_central* const clk_en_cent = (volatile sysctl_clock_enable_central*) CONF_SYSCTL_REGION_BASE + K210_SYSCTL_EN_CENT;
volatile sysctl_clock_enable_peripheral* const clk_en_peri = (volatile sysctl_clock_enable_peripheral*) CONF_SYSCTL_REGION_BASE + K210_SYSCTL_EN_PERI;

static int k210_fpioa_init(void){

	// enable bus clock
	clk_en_cent->apb0 = 1;

	// enable device clock
	clk_en_peri->fpioa = 1;

	return 0;
}