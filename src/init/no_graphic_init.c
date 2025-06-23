/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */

#include <fcntl.h>  // for O_RDONLY
#include <unistd.h> // for open/read/close
#include <devicetree/libfdt.h>
#include <kernel/printk.h> // for printk

void probe_uart_from_dtb_file(void);

int graphic_init(void) {
	printk("Booting Embox...\n");

	probe_uart_from_dtb_file(); // <- call your DTB function here
	return 0;
}

void probe_uart_from_dtb_file(void) {
	int fd = open("/stm32f4_uart.dtb", O_RDONLY);
	if (fd < 0) {
		printk("Failed to open DTB file\n");
		return;
	}

	char buf[4096]; // Adjust size if needed
	int len = read(fd, buf, sizeof(buf));
	close(fd);

	if (len <= 0) {
		printk("Failed to read DTB\n");
		return;
	}

	const void *fdt = (const void *)buf;
	int node = fdt_path_offset(fdt, "/soc/serial@40011000");
	if (node >= 0) {
		const char *compat = fdt_getprop(fdt, node, "compatible", NULL);
		if (compat) {
			printk("UART node compatible: %s\n", compat);
		}
		else {
			printk("UART node has no 'compatible' property\n");
		}
	}
	else {
		printk("UART node not found in DTB\n");
	}
}
