/**
 * @file
 *
 * @date June 20, 2025
 * @author: Shankari Anand
 */

#include <fcntl.h>  // for O_RDONLY
#include <unistd.h> // for open/read/close
#include <libfdt/include/libfdt.h>
#include <kernel/printk.h> // for printk
#include <devicetree/devicetree.h>
#include <stdint.h>

// Externally defined by including stm32f4_uart_dtb.c (xxd -i)
extern const unsigned char stm32f4_uart_dtb[];
extern const unsigned int stm32f4_uart_dtb_len;

static void probe_uart_from_dtb_blob(const void *fdt) {
    int node, len;

    // Locate the UART node by absolute path
    node = fdt_path_offset(fdt, "/soc/serial@40011000");
    if (node < 0) {
        printk("UART node not found in DTB\n");
        return;
    }

    // Compatible string
    const char *compat = fdt_getprop(fdt, node, "compatible", &len);
    if (compat) {
        printk("UART compatible: %s\n", compat);
    } else {
        printk("UART node missing 'compatible'\n");
    }

    // Register base + size
    const fdt32_t *reg = fdt_getprop(fdt, node, "reg", &len);
    if (reg && len >= 8) { // 2 cells (base + size)
        uint32_t base = fdt32_to_cpu(reg[0]);
        uint32_t size = fdt32_to_cpu(reg[1]);
        printk("UART reg: base=0x%x, size=0x%x\n", base, size);
    } else {
        printk("UART node missing/invalid 'reg'\n");
    }

    // Interrupt
    const fdt32_t *irq = fdt_getprop(fdt, node, "interrupts", &len);
    if (irq && len >= 4) {
        uint32_t irq_num = fdt32_to_cpu(irq[0]);
        printk("UART IRQ: %u\n", irq_num);
    } else {
        printk("UART node missing/invalid 'interrupts'\n");
    }

    // Baudrate
    const fdt32_t *baud = fdt_getprop(fdt, node, "current-speed", &len);
    if (baud && len >= 4) {
        uint32_t baudrate = fdt32_to_cpu(baud[0]);
        printk("UART baudrate: %u\n", baudrate);
    } else {
        printk("UART node missing/invalid 'current-speed'\n");
    }
}

int devicetree_init(void) {
    printk("Booting Embox with Device Tree...\n");

    const void *fdt = (const void *)stm32f4_uart_dtb;

    if (fdt_check_header(fdt) != 0) {
        printk("Invalid DTB header\n");
        return -1;
    }

    probe_uart_from_dtb_blob(fdt);
    return 0;
}