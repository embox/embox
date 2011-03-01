/**
 * @file
 *
 * @date 05.01.11
 * @author Anton Bondarev
 */

#include <types.h>
#include <embox/device.h>
#include <fs/file.h>
#include <drivers/tty.h>
#include <kernel/irq.h>

static tty_device_t tty;

static void  *open(const char *fname, const char *mode);
static int    close(void *file);
static size_t read(void *buf, size_t size, size_t count, void *file);
static size_t write(const void *buff, size_t size, size_t count, void *file);

static file_operations_t file_op = {
	.fread = read,
	.fopen = open,
	.fclose = close,
	.fwrite = write
};

static irq_return_t irq_handler(irq_nr_t irq_nr, void *data) {
	int scancode = keyboard_get_scancode();

	tty_add_char(&tty, scancode);
	return IRQ_HANDLED;
}

/*
 * file_operation
 */
static void *open(const char *fname, const char *mode) {
	tty.file_op = &file_op;
	tty_register(&tty);

	irq_attach(1, irq_handler, 0, NULL, "kbd");
	return (void *)&file_op;
}

static int close(void *file) {
	tty_unregister(&tty);
	irq_detach(1, NULL);
	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {
	//TODO if we havn't irq
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

EMBOX_DEVICE("tty", &file_op);

