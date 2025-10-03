/**
 * @file
 *
 * @date August 27, 2025
 * @author Shankari Anand
 */

#include <fcntl.h>   // for O_RDONLY
#include <unistd.h>  // for open/read/close
#include <libfdt/include/libfdt.h>
#include <kernel/printk.h>
#include <stdint.h>

// Externally defined by including beaglevfire.dtb.c (via xxd -i)
extern const unsigned char beaglevfire_dtb[];
extern const unsigned int beaglevfire_dtb_len;

static void probe_uart_from_dtb_blob(const void *fdt) {
    int node, len;

    // Locate UART node (per DTS: /soc/serial@10000000)
    node = fdt_path_offset(fdt, "/soc/serial@10000000");
    if (node < 0) {
        printk("UART node not found in DTB\n");
        return;
    }

    const char *compat = fdt_getprop(fdt, node, "compatible", &len);
    if (compat) {
        printk("UART compatible: %s\n", compat);
    }

    const fdt32_t *reg = fdt_getprop(fdt, node, "reg", &len);
    if (reg && len >= 8) {
        uint64_t base = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) |
                         fdt32_to_cpu(reg[1]);
        uint32_t size = fdt32_to_cpu(reg[2]);
        printk("UART reg: base=0x%llx, size=0x%x\n", base, size);
    }

    const fdt32_t *irq = fdt_getprop(fdt, node, "interrupts", &len);
    if (irq && len >= 4) {
        uint32_t irq_num = fdt32_to_cpu(irq[0]);
        printk("UART IRQ: %u\n", irq_num);
    }
}

static void probe_gpio_from_dtb_blob(const void *fdt) {
    int node, len;

    // Locate GPIO node (per DTS: /soc/gpio@10060000)
    node = fdt_path_offset(fdt, "/soc/gpio@10060000");
    if (node < 0) {
        printk("GPIO node not found in DTB\n");
        return;
    }

    const char *compat = fdt_getprop(fdt, node, "compatible", &len);
    if (compat) {
        printk("GPIO compatible: %s\n", compat);
    }

    const fdt32_t *reg = fdt_getprop(fdt, node, "reg", &len);
    if (reg && len >= 8) {
        uint64_t base = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) |
                         fdt32_to_cpu(reg[1]);
        uint32_t size = fdt32_to_cpu(reg[2]);
        printk("GPIO reg: base=0x%llx, size=0x%x\n", base, size);
    }
}

int devicetree_init(void) {
    printk("Booting Embox (RISC-V) with Device Tree...\n");

    const void *fdt = (const void *)beaglevfire_dtb;

    if (fdt_check_header(fdt) != 0) {
        printk("Invalid DTB header\n");
        return -1;
    }

    probe_uart_from_dtb_blob(fdt);
    probe_gpio_from_dtb_blob(fdt);

    return 0;
}
