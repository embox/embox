#include <gen_board_conf.h>

#include <soc/niiet_k1921vg1t_chip.h>

#define HSECLK_VAL    27000000UL
#define SYSTEM_FREQ  150000000UL

#define PLL0_DIV_NDIV      (100)
#define PLL0_DIV_PREDIV    (1)
#define PLL0_DIV_RDIV      (2)
#define PLL0_DIV_DIV1A     (1)
#define PLL0_DIV_DIV1B     (2)
#define PLL0_FRAC          (1)
#define PLL0_MOD           (1)

//FOUT = 60 000 000 Hz  from 27 MHz HSE
#define PLL1_DIV_NDIV      (100)
#define PLL1_DIV_PREDIV    (1)
#define PLL1_DIV_RDIV      (2)
#define PLL1_DIV_DIV1A     (4)
#define PLL1_DIV_DIV1B     (2)
#define PLL1_FRAC          (1)
#define PLL1_MOD           (1)

/*
    //FOUT = 150 000 000 Hz  from 27 MHz HSE
    RCU->PLL[0].DIV = ( 1 << RCU_PLL_DIV_DIV1A_Pos ) |
                  ( 2 << RCU_PLL_DIV_DIV1B_Pos ) |
                  ( 1 << RCU_PLL_DIV_PREDIV_Pos) |
                  ( 1 << RCU_PLL_DIV_NNCLR_Pos ) |             // N-divider enable
                  ( 1 << RCU_PLL_DIV_RNCLR_Pos ) |             // R-divider enable
                  ( 2 << RCU_PLL_DIV_RDIV_Pos  ) |
                  (100 << RCU_PLL_DIV_NDIV_Pos );
	RCU->PLL[0].MOD  = (1 <<  RCU_PLL_MOD_MOD_Pos );
	RCU->PLL[0].FRAC = (1 << RCU_PLL_FRAC_FRAC_Pos );
	RCU->PLL[0].CFG  = (1 << RCU_PLL_CFG_FOUTEN_Pos ) |			// Fout enable
			       (3 << RCU_PLL_CFG_PFD_Pos    ) |
			       (0 << RCU_PLL_CFG_CLKSEL_Pos ) |
			       (1 << RCU_PLL_CFG_VCOMODE_Pos) |
			       (0 << RCU_PLL_CFG_ST_Pos) ;				  // ST = 0 for integer divider
*/
struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCU",
			.regs = {
				REGMAP("BASE", (RCU_BASE), 0x100),
			},
			.clocks = {
				VAL("HSECLK_VAL", HSECLK_VAL),

				VAL("PLL0_DIV_NDIV", PLL0_DIV_NDIV),
				VAL("PLL0_DIV_PREDIV", PLL0_DIV_PREDIV),
				VAL("PLL0_DIV_RDIV", PLL0_DIV_RDIV),
				VAL("PLL0_DIV_DIV1A", PLL0_DIV_DIV1A),
				VAL("PLL0_FRAC", PLL0_FRAC),
				VAL("PLL0_MOD", PLL0_MOD),

				VAL("PLL1_DIV_NDIV", PLL1_DIV_NDIV),
				VAL("PLL1_DIV_PREDIV", PLL1_DIV_PREDIV),
				VAL("PLL1_DIV_RDIV", PLL1_DIV_RDIV),
				VAL("PLL1_DIV_DIV1A", PLL1_DIV_DIV1A),
				VAL("PLL1_FRAC", PLL1_FRAC),
				VAL("PLL1_MOD", PLL1_MOD),
			},
		},
		.type = {
			VAL("SYSCLK_PLL", 1),
			VAL("USB_PLL_NUM", 1),
		},
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "SIU",
			.regs = {
				REGMAP("BASE", (SIU_BASE), 0x100),
			},
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", (GPIOA_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOA),
			},
			.clocks = {
				VAL("", "CLK_GPIOA"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_B",
			.regs = {
				REGMAP("BASE", (GPIOB_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOB),
			},
			.clocks = {
				VAL("", "CLK_GPIOB"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[2] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_C",
			.regs = {
				REGMAP("BASE", (GPIOC_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOC),
			},
			.clocks = {
				VAL("", "CLK_GPIOC"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[3] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_D",
			.regs = {
				REGMAP("BASE", (GPIOD_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOD),
			},
			.clocks = {
				VAL("", "CLK_GPIOD"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[4] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_E",
			.regs = {
				REGMAP("BASE", (GPIOE_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOE),
			},
			.clocks = {
				VAL("", "CLK_GPIOE"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[5] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_F",
			.regs = {
				REGMAP("BASE", (GPIOF_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOF),
			},
			.clocks = {
				VAL("", "CLK_GPIOF"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[6] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_G",
			.regs = {
				REGMAP("BASE", (GPIOG_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOG),
			},
			.clocks = {
				VAL("", "CLK_GPIOG"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
#if 0
	[7] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_H",
			.regs = {
				REGMAP("BASE", (GPIOH_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOH),
			},
			.clocks = {
				VAL("", "CLK_GPIOH"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
#endif
};

struct uart_conf uarts[] = {
	[0] = {
		.status = ENABLED,
		.name = "UART0",
		.dev = {
			.name = "UART0",
			.regs = {
				REGMAP("BASE_ADDR", (UART0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_UART0),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 1, GPIO_MODE_ALT, 3),
				PIN("RX", GPIO_PORT_B, 0, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("", "CLK_UART0"),
			}
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = ENABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", (UART1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_UART1),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 5, GPIO_MODE_ALT, 3),
				PIN("RX", GPIO_PORT_B, 4, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("", "CLK_UART1"),
			}
		},
		.baudrate = 115200,
	},
	[2] = {
		.status = ENABLED,
		.name = "UART2",
		.dev = {
			.name = "UART2",
			.regs = {
				REGMAP("BASE_ADDR", (UART2_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_UART2),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 9, GPIO_MODE_ALT, 3),
				PIN("RX", GPIO_PORT_B, 8, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("", "CLK_UART2"),
			}
		},
		.baudrate = 115200,
	},
	[3] = {
		.status = ENABLED,
		.name = "UART3",
		.dev = {
			.name = "UART3",
			.regs = {
				REGMAP("BASE_ADDR", (UART3_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_UART3),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 13, GPIO_MODE_ALT, 3),
				PIN("RX", GPIO_PORT_B, 12, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("", "CLK_UART3"),
			}
		},
		.baudrate = 115200,
	},
	[4] = {
		.status = ENABLED,
		.name = "UART4",
		.dev = {
			.name = "UART4",
			.regs = {
				REGMAP("BASE_ADDR", (UART4_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_UART4),
			},
			.pins = {
				PIN("TX", GPIO_PORT_E, 1, GPIO_MODE_ALT, 1),
				PIN("RX", GPIO_PORT_E, 0, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("", "CLK_UART3"),
			}
		},
		.baudrate = 115200,
	},


};

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.dev = {
			.name = "PWM0",
			.regs = {
				REGMAP("BASE_ADDR", (TMR0_BASE), 0x100),
			},
			.pins = {
				// PIN("OUT0", GPIO_PORT_A, 0, GPIO_MODE_ALT, 5),
				// PIN("OUT1", GPIO_PORT_A, 1, GPIO_MODE_ALT, 5),
				// PIN("OUT2", GPIO_PORT_A, 2, GPIO_MODE_ALT, 5),
				// PIN("OUT3", GPIO_PORT_A, 3, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR0"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR0"),
				VAL("TMR_IDX", 0),
			},
		},
	},
	[1] = {
		.name = "PWM1",
		.dev = {
			.name = "PWM1",
			.regs = {
				REGMAP("BASE_ADDR", (TMR1_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_A, 4, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_A, 5, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_A, 6, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_A, 7, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR1"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR1"),
				VAL("TMR_IDX", 1),
			},
		},
	},
	[2] = {
		.name = "PWM2",
		.dev = {
			.name = "PWM2",
			.regs = {
				REGMAP("BASE_ADDR", (TMR2_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_G, 0, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_G, 1, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_G, 2, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_G, 3, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR2"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR2"),
				VAL("TMR_IDX", 2),
			},
		},
	},
	[3] = {
		.name = "PWM3",
		.dev = {
			.name = "PWM3",
			.regs = {
				REGMAP("BASE_ADDR", (TMR3_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_G, 4, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_G, 5, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_G, 6, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_G, 7, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR3"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR3"),
				VAL("TMR_IDX", 3),
			},
		},
	},
	[10] = {
		.name = "PWM10",
		.dev = {
			.name = "PWM10",
			.regs = {
				REGMAP("BASE_ADDR", (TMR10_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_D, 5, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("",  "CLK_TMR10"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR10"),
				VAL("TMR_IDX", 10),
			},
		},
	},
	[12] = {
		.name = "PWM12",
		.dev = {
			.name = "PWM12",
			.regs = {
				REGMAP("BASE_ADDR", (TMR12_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_D, 6, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR12"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR12"),
				VAL("TMR_IDX", 12),
			},
		},
	},
	[13] = {
		.name = "PWM13",
		.dev = {
			.name = "PWM13",
			.regs = {
				REGMAP("BASE_ADDR", (TMR13_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_D, 8, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR13"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR13"),
				VAL("TMR_IDX", 13),
			},
		},
	},
	[14] = {
		.name = "PWM14",
		.dev = {
			.name = "PWM14",
			.regs = {
				REGMAP("BASE_ADDR", (TMR14_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_D, 7, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR143"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR14"),
				VAL("TMR_IDX", 14),
			},
		},
	},
};

struct spi_conf spis[] = {
	[0] = {
		.status = ENABLED,
		.name = "SPI0",
		.dev = {
			.name = "SPI0",
			.regs = {
				REGMAP("BASE_ADDR", (SPI0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_SPI0),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 0, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 2, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 3, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI0"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI0_0",
			.bits_per_word = 8,
			.bus_num = 0,
			.idx     = 0,
			.dev = {
				.name = "SPI0_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 1, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.regs = {
				REGMAP("BASE_ADDR", (SPI1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_SPI1),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 4, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 6, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 7, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI1"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI1_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI1_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 5, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
	[2] = {
		.status = ENABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.regs = {
				REGMAP("BASE_ADDR", (SPI2_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_SPI2),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 8, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 10, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 11, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI2"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI2_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI2_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 5, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
	[3] = {
		.status = ENABLED,
		.name = "SPI3",
		.dev = {
			.name = "SPI3",
			.regs = {
				REGMAP("BASE_ADDR", (SPI3_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_SPI3),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 12, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 14, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 15, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI3"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI3_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI3_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 13, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
};

struct i2c_conf i2cs[] = {
	[0] = {
		.status = ENABLED,
		.name = "I2C0",
		.dev = {
			.name = "I2C0",
			.regs = {
				REGMAP("BASE_ADDR", (I2C0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_I2C0),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_D, 3, GPIO_MODE_ALT, 2),
				PIN("SDA", GPIO_PORT_D, 2, GPIO_MODE_ALT, 2),
			},
			.clocks = {
				VAL("", "CLK_I2C0"),
			}
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.regs = {
				REGMAP("BASE_ADDR", (I2C1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_I2C1),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_D, 1, GPIO_MODE_ALT, 1),
				PIN("SDA", GPIO_PORT_D, 0, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("", "CLK_I2C1"),
			}
		},
	},

};

struct can_conf cans[] = {
	[0] = {
		.status = ENABLED,
		.name = "CAN0",
		.dev = {
			.name = "CAN0",
			.regs = {
				REGMAP("BASE", (CANFD0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_CANFD01),
			},
			.pins = {
				PIN("CAN_RX", GPIO_PORT_A, 0, GPIO_MODE_ALT, 3),
				PIN("CAN_TX", GPIO_PORT_A, 1, GPIO_MODE_ALT, 3),
				// PIN("CAN_RX", GPIO_PORT_D, 8, GPIO_MODE_ALT, 1),
				// PIN("CAN_TX", GPIO_PORT_D, 9, GPIO_MODE_ALT, 1),

			},
			.clocks = {
				VAL("", "CLK_CAN0"),
			},
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "CAN1",
		.dev = {
			.name = "CAN1",
			.regs = {
				REGMAP("BASE", (CANFD1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_CANFD1),
			},
			.pins = {
				PIN("CAN_RX", GPIO_PORT_A, 2, GPIO_MODE_ALT, 3),
				PIN("CAN_TX", GPIO_PORT_A, 3, GPIO_MODE_ALT, 3),
				// PIN("CAN_RX", GPIO_PORT_D, 10, GPIO_MODE_ALT, 1),
				// PIN("CAN_TX", GPIO_PORT_D, 1, GPIO_MODE_ALT, 1),

			},
			.clocks = {
				VAL("", "CLK_CAN1"),
			},
		},
	},
	[2] = {
		.status = ENABLED,
		.name = "CAN2",
		.dev = {
			.name = "CAN2",
			.regs = {
				REGMAP("BASE", (CANFD2_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_CANFD2),
			},
			.pins = {
				PIN("CAN_RX", GPIO_PORT_A, 4, GPIO_MODE_ALT, 3),
				PIN("CAN_TX", GPIO_PORT_A, 5, GPIO_MODE_ALT, 3),
				// PIN("CAN_RX", GPIO_PORT_D, 12, GPIO_MODE_ALT, 1),
				// PIN("CAN_TX", GPIO_PORT_D, 13, GPIO_MODE_ALT, 1),

			},
			.clocks = {
				VAL("", "CLK_CAN2"),
			},
		},
	},
	[3] = {
		.status = ENABLED,
		.name = "CAN3",
		.dev = {
			.name = "CAN3",
			.regs = {
				REGMAP("BASE", (CANFD3_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_CANFD3),
			},
			.pins = {
				PIN("CAN_RX", GPIO_PORT_A, 6, GPIO_MODE_ALT, 3),
				PIN("CAN_TX", GPIO_PORT_A, 7, GPIO_MODE_ALT, 3),
				// PIN("CAN_RX", GPIO_PORT_D, 14, GPIO_MODE_ALT, 1),
				// PIN("CAN_TX", GPIO_PORT_D, 15, GPIO_MODE_ALT, 1),

			},
			.clocks = {
				VAL("", "CLK_CAN1"),
			},
		},
	},
};

struct usb_conf usbs[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "USB0",
			.regs = {
				REGMAP("USBDC_BASE", (USBDC0_BASE), 0x1000),
				REGMAP("USBCTR_BASE", (USBCTR0_BASE), 0x1000),
				REGMAP("USBHC_BASE", (USBHC0_BASE), 0x1000),
			},
			.irqs = {
				VAL("", PLIC_IRQ_USB0),
			},
			.pins = {
			},
			.clocks = {
				VAL("USB", "CLK_USBD0"),
			},
			.misc = {
				VAL("PHY_EXT", 0),
				VAL("PHY_POLARITY", 0),
				VAL("TYPE_FS", 1),
				VAL("EP_MAX", 4),
				VAL("EP_MAX_SIZE", 64),
			},
		},
	},
#if 0
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "USB1",
			.regs = {
				REGMAP("USBDC_BASE", (USBDC1_BASE), 0x1000),
				REGMAP("USBCTR_BASE", (USBCTR1_BASE), 0x1000),
				REGMAP("USBHC_BASE", (USBHC1_BASE), 0x1000),
			},
			.irqs = {
				VAL("", PLIC_IRQ_USB1),
			},
			.pins = {

			},
			.clocks = {
				VAL("USB", "CLK_USBD1"),
			},
			.misc = {
				VAL("TYPE_FS", 1),
				VAL("EP_MAX", 4),
				VAL("EP_MAX_SIZE", 64),
			},
		},
	},
#endif
};


struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LED1",
		.port = VAL("", GPIO_PORT_F),
		.pin = VAL("", 10),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,	
		.name = "LED2",
		.port = VAL("", GPIO_PORT_F),
		.pin = VAL("", 11),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.status = ENABLED,		
		.name = "LED3",
		.port = VAL("", GPIO_PORT_F),
		.pin = VAL("", 12),
		.level = VAL("", GPIO_PIN_HIGH),
	},

};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds),
					PWM(pwms), SPI(spis), I2C(i2cs), CAN(cans), USB(usbs))
