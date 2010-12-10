
#include <drivers/tty_action.h>

void tty_write_space( tty_device_t *tty, uint32_t cnt ) {
	for (; cnt>0; --cnt) {
		tty->file_op->fwrite(" ", sizeof(char), 1, NULL);
	}
}

void tty_go_left( tty_device_t *tty, uint32_t cnt ) {
	for (; cnt>0; --cnt) {
		vtbuild( tty->vtb, TOKEN_LEFT );
	}
}

void tty_go_right( tty_device_t *tty, uint32_t cnt ) {
	for (; cnt>0; --cnt) {
		vtbuild( tty->vtb, TOKEN_RIGHT );
	}
}

void tty_go_cursor_position( tty_device_t *tty, uint32_t cur_before,  uint32_t cur_after ) {
	if (cur_before<cur_after) {
		tty_go_right( tty, cur_after - cur_before );
	} else {
		tty_go_left( tty, cur_before - cur_after );
	}
}

void tty_clean_line( tty_device_t *tty, uint32_t cur_before, uint32_t length, uint32_t cur_after ) {
	tty_go_left( tty, cur_before );
	tty_write_space( tty , length );
	tty_go_left( tty, length - cur_after );
}

void tty_write_line( tty_device_t *tty, uint8_t *line, uint32_t size, uint32_t cur_after ) {
	tty->file_op->fwrite( line, sizeof(uint8_t), size, NULL );
	tty_go_left( tty, size - cur_after );
}

void tty_rewrite_line( tty_device_t *tty, uint8_t *line, uint32_t size, uint32_t old_size, uint32_t cur_before, uint32_t cur_after ) {
	tty_go_left( tty, cur_before );
	if (old_size>size) {
		tty_write_space( tty, old_size );
		tty_go_left( tty, old_size );
	}
	tty->file_op->fwrite( line, sizeof(uint8_t), size, NULL );
	#if 0
	int i;
	for (i=0;i<size;++i) {
	tty->file_op->fwrite( &line[i], sizeof(uint8_t), 1, NULL );
	printf("wtf?");
	printf(" s: %d ",size);
	}
	#endif
	tty_go_left( tty, size - cur_after );
}

void copy_forward( uint8_t *s, uint8_t *d, uint32_t size ) {
	uint32_t i;
	for (i=0; i<size; ++i) {
		d[i] = s[i];
	}
}

void copy_backward( uint8_t *s, uint8_t *d, uint32_t size ) {
	for (; size>0; --size) {
		d[size] = s[size];
	}
	d[0] = s[0];
}

