#ifndef MEMORY_MAP_5345_87687689_INCLUDE
#define MEMORY_MAP_5345_87687689_INCLUDE


// Memory map
#define ROM_BASE    	0x00000000
#define IO_BASE     	0x20000000
#define SDRAM_BASE  	0x40000000
#define SDRAM_END   	(SDRAM_BASE+SDRAM_SIZE-1)
#define SDRAM_BANK_SIZE	0x2000000
#define RAM_BASE    	0xA0000000
#define RG_BASE     	0x80000000//LEON REG BASE
#define DSU_BASE    	0x90000000

#ifdef LEON2
#define UART_BASE		0x80000070
#define TIMERS_BASE		0x80000040
#define IRQ_REGS_BASE	0x80000090
#endif

#ifdef LEON3
#define UART_BASE		0x80000100
#define IRQ_REGS_BASE	0x80000200
#define TIMERS_BASE		0x80000300
#endif


/* KTPO data base address */
#define DATA_BASE   SDRAM_BASE    // stack in SDRAM

/* Data base address  */
#define DATA_SIZE   0x20000

/* Stack base address (stack grows down form base) */
#define STACK_BASE  DATA_BASE + DATA_SIZE

#endif
