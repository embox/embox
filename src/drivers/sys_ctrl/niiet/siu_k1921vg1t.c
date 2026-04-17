/**
 * @file
 *
 * @date 07.04.2026
 * @author Anton Bondarev
 */

struct siu_regs {
	volatile const uint32_t CHIPID_reg; /*!< Chip identifier */

	volatile uint32_t SERVCTL_reg; /*!< Service mode register */

	volatile const uint32_t CPESTAT; /*!< CPE status register */

	volatile uint32_t LOCK; /*!< Lock configuration register */

	volatile uint32_t CORE1_CLKEN; /*!< Core 1 clock enable register */

	volatile uint32_t CORE1_RSTDIS; /*!< Core 1 reset disable register */

	volatile uint32_t CORE1_RVECT; /*!< Core 1 reset vector register */

	volatile uint32_t CORE1_CONF; /*!< Core 1 configuration register */

	volatile uint32_t SSRAMSCR; /*!< System SRAM sleep control register */

	volatile uint32_t SSRAMMCR; /*!< System SRAM margin control register */

	volatile uint32_t PSRAMSMCR; /*!< Periperals SRAM sleep and margin control register */

	volatile uint32_t CCSMCR; /*!< Core caches sleep and margin control register */

	volatile uint32_t INTEN; /*!< Interrupt enable register */

	volatile const uint32_t RIS; /*!< Raw interrupt status register */

	volatile const uint32_t MIS; /*!< Masked interrupt status register */

	volatile uint32_t ICLR; /*!< Interrupt clear register */

	volatile uint32_t RCCTRL; /*!< RC generator control register */

	volatile uint32_t LDOCTRL; /*!< LDO control register */

	volatile uint32_t BGPCTRL; /*!< Bandgap control register */

	volatile uint32_t TSENSCTRL; /*!< Tempreture sensor control register */

	volatile uint32_t TMRCFG; /*!< Timers external clock config */

	volatile uint32_t TMREN; /*!< Timers counting enable register */

	volatile uint32_t PWMEN; /*!< PWM counting enable register */

	volatile uint32_t PWMSYNC; /*!< Sync PWM register */

	volatile uint32_t EMMUXCTRL; /*!< Extrernal memory multiplexor control register */

	volatile uint32_t TIMEOUT; /*!< Timeout register */

	volatile uint32_t FRSTEN; /*!< Fault reset enable register */

	volatile uint32_t FSTAT; /*!< Fault status regsiter */

	volatile uint32_t TMRMUX[16];
	volatile uint32_t Reserved0[20];

	volatile uint32_t NODESTEST; /*!< Nodes selftest start and status register */

	volatile uint32_t Reserved1[3];

	volatile const uint32_t NODE0ECNT; /*!< Node 0 error counter register */

	volatile uint32_t NODE0CTRL; /*!< Node 0 control register */

	volatile const uint32_t NODE1ECNT; /*!< Node 1 error counter register */

	volatile uint32_t NODE1CTRL; /*!< Node 1 control register */

	volatile const uint32_t NODE2ECNT; /*!< Node 2 error counter register */

	volatile uint32_t NODE2CTRL; /*!< Node 2 control register */

	volatile const uint32_t NODE3ECNT; /*!< Node 3 error counter register */

	volatile uint32_t NODE3CTRL; /*!< Node 3 control register */

	volatile const uint32_t NODE4ECNT; /*!< Node 4 error counter register */

	volatile uint32_t NODE4CTRL; /*!< Node 4 control register */
};
