/**
 * @file
 * @brief Creates diag file in /dev/
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/file.h>
#include <kernel/diag.h>
#include <embox/device.h>

static void *open(const char *fname, const char *mode);
static int close(void *file);
static size_t read(void *buf, size_t size, size_t count, void *file);
static size_t write(const void *buff, size_t size, size_t count, void *file);

static file_operations_t file_op = {
		.fread = read,
		.fopen = open,
		.fclose = close,
		.fwrite = write
};

/*
 * file_operation
 */
static void *open(const char *fname, const char *mode) {
#ifdef CONFIG_TTY_DEVICE //XXX KILL-ME
	tty.file_op = &file_op;
	tty_register(&tty);
	uart_set_irq_handler(irq_handler);
#endif
	return (void *)&file_op;
}

static int close(void *file) {
#ifdef CONFIG_TTY_DEVICE //XXX KILL-ME
	tty_unregister(&tty);
	uart_remove_irq_handler();
#endif
	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;

	int i = count * size;

	while (i --) {
		*(ch_buf++) = diag_getc();
	}

	return 0;
}

static size_t write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;

	while (cnt != count * size) {
		diag_putc(b[cnt++]);
	}
	return 0;
}

EMBOX_DEVICE("diag", &file_op);
