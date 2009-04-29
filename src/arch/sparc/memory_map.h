#ifndef MEMORY_MAP_5345_87687689_INCLUDE
#define MEMORY_MAP_5345_87687689_INCLUDE


// Memory map
#define ROM_BASE    	0x00000000
#define IO_BASE     	0x20000000
#define SDRAM_BASE  	0x40000000
//#define SDRAM_END   	(SDRAM_BASE+SDRAM_SIZE-1)
#define SDRAM_BANK_SIZE	0x2000000
#define RAM_BASE    	0xA0000000
#define RG_BASE     	0x80000000//LEON REG BASE
#define DSU_BASE    	0x90000000

#ifdef LEON2
#define UART_BASE		0x80000070
#define IRQ_REGS_BASE	0x80000090
#endif

#ifdef LEON3
#define UART_BASE		0x80000100
#define IRQ_REGS_BASE	0x80000200
#endif


/* KTPO data base address */
#ifndef SIMULATE
#define DATA_BASE   SDRAM_BASE    // stack in SDRAM
#else
#define DATA_BASE   RAM_BASE    // stack in SDRAM
#endif //SIMULATE
/* Data base address  */
#ifndef SIMULATE
#define DATA_SIZE   0x20000
#else
#define DATA_SIZE   0x80000 - 4
#endif //SIMULATE
/* Stack base address (stack grows down form base) */
#define STACK_BASE  DATA_BASE + DATA_SIZE


#endif
