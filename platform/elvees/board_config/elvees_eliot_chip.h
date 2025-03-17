#include <gen_board_conf.h>

#define NS_PERIPHERAL_BASE          (0x40000000UL)

#define SECURE_PERIPHERAL_BASE      (0x50000000UL)


#define CPU_BUS_BASE            (PERIPHERAL_BASE + 0x00010000UL)

#define CPU_IDENTITY_BASE       (CPU_BUS_BASE + 0x0000F000UL)

#define APB_PPC3_BASE           (PERIPHERAL_BASE + 0x00030000UL)

#define IOCTR_BASE              (APB_PPC3_BASE + 0x00000000UL)
#define CLKCTR_BASE             (APB_PPC3_BASE + 0x00001000UL)
#define PWRCTR_BASE             (APB_PPC3_BASE + 0x00002000UL)

/* APB_PPC0 */
#define APB_PPC0_BASE           (PERIPHERAL_BASE + 0x00100000UL)

#define UART0_BASE              (APB_PPC0_BASE + 0x00000000UL)
#define UART1_BASE              (APB_PPC0_BASE + 0x00001000UL)
#define UART2_BASE              (APB_PPC0_BASE + 0x00002000UL)
#define UART3_BASE              (APB_PPC0_BASE + 0x00003000UL)

#define SPI0_BASE               (APB_PPC0_BASE + 0x00004000UL)
#define SPI1_BASE               (APB_PPC0_BASE + 0x00005000UL)
#define SPI2_BASE               (APB_PPC0_BASE + 0x00006000UL)

#define I2C0_BASE               (APB_PPC0_BASE + 0x00007000UL)
#define I2C1_BASE               (APB_PPC0_BASE + 0x00008000UL)

#define I2S_BASE                (APB_PPC0_BASE + 0x00009000UL)

#define GPIO0_BASE              (APB_PPC0_BASE + 0x0000A000UL)
#define GPIO1_BASE              (APB_PPC0_BASE + 0x0000B000UL)
#define GPIO2_BASE              (APB_PPC0_BASE + 0x0000C000UL)
#define GPIO3_BASE              (APB_PPC0_BASE + 0x0000D000UL)

#define IRQ_NUM_UART0           32
#define IRQ_NUM_UART1           33
#define IRQ_NUM_UART2           34
#define IRQ_NUM_UART3           35

#define IRQ_NUM_SPI0            36
#define IRQ_NUM_SPI1            37
#define IRQ_NUM_SPI2            38

#define IRQ_NUM_I2C0            39
#define IRQ_NUM_I2C1            40

#define IRQ_NUM_I2S             41

#define IRQ_NUM_GPIO0_S         42
#define IRQ_NUM_GPIO1_S         43
#define IRQ_NUM_GPIO2_S         44
#define IRQ_NUM_GPIO3_S         45


#define AF(x)                   (x)
