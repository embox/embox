#include <types.h>
#include <drivers/tty_action.h>
#include <ctype.h>

/* auxilary functions */

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

/* actions in tty */

/* print char */
void tac_key_alpha( tty_device_t *tty, struct vt_token *token ) {
	if (tty->ins_mod) { /* INSERT MODE */
		if (tty->rx_cnt < TTY_RXBUFF_SIZE) {
			++tty->rx_cnt;
			copy_backward(
				&tty->rx_buff[tty->rx_cur],
				&tty->rx_buff[tty->rx_cur+1],
				tty->rx_cnt - tty->rx_cur);
			tty->rx_buff[tty->rx_cur++] = token->ch;
			vtbuild( tty->vtb, token );
			tty_write_line( tty, &tty->rx_buff[tty->rx_cur],
				tty->rx_cnt - tty->rx_cur, 0);
		}
	} else { /* REPLACE MOD */
		if (tty->rx_cur < TTY_RXBUFF_SIZE) {
			tty->rx_buff[tty->rx_cur++] = token->ch;
			if ( tty->rx_cur > tty->rx_cnt ) {
				tty->rx_cnt = tty->rx_cur;
			}
			vtbuild(tty->vtb, token);
		}
	}
}

/* remove one char */
void tac_key_del( tty_device_t *tty ) {
	if (cur_tty->rx_cur<cur_tty->rx_cnt) {
		copy_forward(
			&cur_tty->rx_buff[cur_tty->rx_cur+1],
			&cur_tty->rx_buff[cur_tty->rx_cur],
			--cur_tty->rx_cnt - cur_tty->rx_cur );
		tty_rewrite_line( cur_tty,
			&cur_tty->rx_buff[cur_tty->rx_cur],
			cur_tty->rx_cnt - cur_tty->rx_cur,
			cur_tty->rx_cnt - cur_tty->rx_cur + 1, 0, 0);
	}
}

/* remove one char */
void tac_key_backspace( tty_device_t *tty ) {
	if (tty->rx_cur>0) {
		--tty->rx_cur;
		vtbuild(tty->vtb, TOKEN_LEFT);
		uint32_t i;
		for (i=tty->rx_cur; i<tty->rx_cnt-1; ++i) {
			tty->rx_buff[i] = tty->rx_buff[i+1];
			tty->file_op->fwrite(&tty->rx_buff[i],sizeof(char),1,NULL);
		}
		tty->file_op->fwrite(" ",sizeof(char),1,NULL);
		for (i=tty->rx_cur; i<tty->rx_cnt; ++i) {
			vtbuild(tty->vtb, TOKEN_LEFT);
		}
		--tty->rx_cnt;
	}
}

/* move cursor left one char */
void tac_key_left( tty_device_t *tty ) {
	if (tty->rx_cur>0) {
		--tty->rx_cur;
		/* vtbuild(tty->vtb, token); */
		tty_go_left( tty, 1 );
	}
}

/* move cursor right one char */
void tac_key_right( tty_device_t *tty ) {
	if (tty->rx_cur < tty->rx_cnt) {
		++tty->rx_cur;
		/* vtbuild(tty->vtb, token); */
		tty_go_right( tty, 1 );
	}
}

/* move cursor to begin of the line */
void tac_key_home( tty_device_t *tty ) {
	tty_go_cursor_position( cur_tty, cur_tty->rx_cur, 0 );
	cur_tty->rx_cur = 0;
}

/* move cursor to end of the line */
void tac_key_end( tty_device_t *tty ) {
	tty_go_cursor_position( cur_tty, cur_tty->rx_cur, cur_tty->rx_cnt );
	cur_tty->rx_cur = cur_tty->rx_cnt;
}

/* switch insert-replace mode */
void tac_key_ins( tty_device_t *tty ) {
	cur_tty->ins_mod = !cur_tty->ins_mod;
}

/* execute command line */
void tac_key_enter( tty_device_t *tty, struct vt_token *token ) {
	if (tty->out_busy) return;
	/* add string to output buffer */
	memcpy((void*) tty->out_buff,(const void*)
		tty->rx_buff, tty->rx_cnt);
	tty->out_buff[tty->rx_cnt] = '\0';
	tty->rx_cnt = 0;
	tty->rx_cur = 0;
	tty->out_busy = true;
	vtbuild(tty->vtb, token);
}

/* remove last word in command line */
void tac_remove_word( tty_device_t *tty ) {
	/* find end of prev word */
	uint32_t tps; /* tps - to position space */
	tps = tty->rx_cur>0 ? tty->rx_cur-1 : 0;
	for (; tps>0 && isalpha(tty->rx_buff[tps]); --tps);
	for (; tps>0 && isspace(tty->rx_buff[tps]); --tps);
	if (tps>0) {++tps;}

	copy_forward(
		&tty->rx_buff[tty->rx_cur],
		&tty->rx_buff[tps],
		tty->rx_cnt - tty->rx_cur + tps );

	tty_rewrite_line( tty,
		&tty->rx_buff[tps],
		tty->rx_cnt - tty->rx_cur ,
		tty->rx_cnt - tps,
		tty->rx_cur - tps, 0 );

	tty->rx_cnt -= tty->rx_cur - tps;
	tty->rx_cur = tps;
}

/* remove the line */
void tac_remove_line( tty_device_t *tty ) {
	copy_forward(
		&tty->rx_buff[tty->rx_cur],
		&tty->rx_buff[0],
		tty->rx_cnt - tty->rx_cur );
	tty_rewrite_line( tty,
		&tty->rx_buff[0],
		tty->rx_cnt - tty->rx_cur,
		tty->rx_cnt, tty->rx_cur, 0);
	tty->rx_cnt -= tty->rx_cur;
	tty->rx_cur = 0;
}

#ifdef CONFIG_TTY_CONSOLE_COUNT
/* switch console */
void tac_switch_console( tty_device_t *tty , uint32_t c_after ) {
	vconsole_deactivate(&tty->console[tty->console_cur]);
	tty->console_cur = c_after;
	tty_go_left( tty, 7 ); /* erase `TTY-X$' */
	printf("TTY-%d$ ",tty->console_cur+1);
	vconsole_activate(&tty->console[tty->console_cur]);
}
#endif


