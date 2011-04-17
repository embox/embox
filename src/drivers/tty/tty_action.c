/**
 * @file
 *
 * @date 07.12.10
 * @author Fedor Burdun
 */

#include <kernel/thread/api.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <drivers/tty_action.h>
#include <kernel/diag.h>

/*
 * FIXME
 * 	buffers is used elements of different sizes. Check it and fix if need.
 */

struct vt_token char_token( char ch ) {
	struct vt_token tmp;
	tmp.action = VT_ACTION_PRINT;
	tmp.ch = ch;
	tmp.attrs_len = 0;
	tmp.params_len = 0;
	/* null to array */
	return tmp;
}

/* auxilary functions */

void tty_write_space(tty_device_t *tty, uint32_t cnt) {
	struct vt_token vt = char_token(' ');
	for (; cnt > 0; --cnt) {
		//tty->file_op->fwrite(" ", sizeof(char), 1, NULL);
		vtbuild((struct vtbuild *)tty->vtb, &vt);
	}
}

void tty_go_left(tty_device_t *tty, uint32_t cnt) {
	for (; cnt > 0; --cnt) {
		vtbuild((struct vtbuild *)tty->vtb, TOKEN_LEFT);
	}
}

void tty_go_right(tty_device_t *tty, uint32_t cnt) {
	for (; cnt > 0; --cnt) {
		vtbuild((struct vtbuild *)tty->vtb, TOKEN_RIGHT);
	}
}

void tty_go_cursor_position(tty_device_t *tty,
		uint32_t cur_before, uint32_t cur_after) {
	if (cur_before < cur_after) {
		tty_go_right(tty, cur_after - cur_before);
	} else {
		tty_go_left(tty, cur_before - cur_after);
	}
}

void tty_clean_line(tty_device_t *tty, uint32_t cur_before,
			uint32_t length, uint32_t cur_after) {
	tty_go_left(tty, cur_before);
	tty_write_space(tty, length);
	tty_go_left(tty, length - cur_after);
}

void tty_write_line(tty_device_t *tty, uint8_t *line,
			uint32_t size, uint32_t cur_after) {
	tty->file_op->fwrite(line, sizeof(uint8_t), size, NULL);
	tty_go_left(tty, size - cur_after);
}

void tty_rewrite_line(tty_device_t *tty, uint8_t *line, uint32_t size,
		uint32_t old_size, uint32_t cur_before, uint32_t cur_after) {
	tty_go_left(tty, cur_before);
	if (old_size > size) {
		tty_write_space(tty, old_size);
		tty_go_left(tty, old_size);
	}
	tty->file_op->fwrite(line, sizeof(uint8_t), size, NULL);
	#if 0
	int i;
	for (i = 0; i < size; ++i) {
		tty->file_op->fwrite(&line[i], sizeof(uint8_t), 1, NULL);
		printf("wtf?");
		printf(" s: %d ", size);
	}
	#endif
	tty_go_left(tty, size - cur_after);
}

void copy_forward(uint8_t *s, uint8_t *d, uint32_t size) {
	size_t i;
	for (i = 0; i < size; ++i) {
		d[i] = s[i];
	}
}

void copy_backward(uint8_t *s, uint8_t *d, uint32_t size) {
	for (; size > 0; --size) {
		d[size] = s[size];
	}
	d[0] = s[0];
}

/* actions in tty */

/* print char */
void tac_key_alpha(tty_device_t *tty, struct vt_token *token) {
	if (tty->ins_mod) { /* INSERT MODE */
		if (tty->rx_cnt < TTY_RXBUFF_SIZE) {
			++tty->rx_cnt;
			copy_backward(
				(uint8_t *) &tty->rx_buff[tty->rx_cur],
				(uint8_t *) &tty->rx_buff[tty->rx_cur + 1],
				(tty->rx_cnt - tty->rx_cur));
			tty->rx_buff[tty->rx_cur++] = token->ch;
			vtbuild((struct vtbuild *)tty->vtb, token);
			tty_write_line(tty, (uint8_t *) &tty->rx_buff[tty->rx_cur],
				tty->rx_cnt - tty->rx_cur, 0);
		}
	} else { /* REPLACE MOD */
		if (tty->rx_cur < TTY_RXBUFF_SIZE) {
			tty->rx_buff[tty->rx_cur++] = token->ch;
			if (tty->rx_cur > tty->rx_cnt) {
				tty->rx_cnt = tty->rx_cur;
			}
			vtbuild((struct vtbuild *)tty->vtb, token);
		}
	}
}

/* remove one char */
void tac_key_del(tty_device_t *tty) {
	if (cur_tty->rx_cur < cur_tty->rx_cnt) {
		copy_forward(
			(uint8_t *) &cur_tty->rx_buff[cur_tty->rx_cur+1],
			(uint8_t *) &cur_tty->rx_buff[cur_tty->rx_cur],
			--cur_tty->rx_cnt - cur_tty->rx_cur);
		tty_rewrite_line(cur_tty,
			(uint8_t *) &cur_tty->rx_buff[cur_tty->rx_cur],
			cur_tty->rx_cnt - cur_tty->rx_cur,
			cur_tty->rx_cnt - cur_tty->rx_cur + 1, 0, 0);
	}
}

/* remove one char */
void tac_key_backspace(tty_device_t *tty) {
	uint32_t i;
	if (tty->rx_cur > 0) {
		--tty->rx_cur;
		vtbuild((struct vtbuild *)tty->vtb, TOKEN_LEFT);
		for (i = tty->rx_cur; i < tty->rx_cnt - 1; ++i) {
			tty->rx_buff[i] = tty->rx_buff[i + 1];
			tty->file_op->fwrite((uint8_t *) &tty->rx_buff[i], sizeof(char), 1, NULL);
		}
		tty->file_op->fwrite(" ", sizeof(char), 1, NULL);
		for (i = tty->rx_cur; i < tty->rx_cnt; ++i) {
			vtbuild((struct vtbuild *)tty->vtb, TOKEN_LEFT);
		}
		--tty->rx_cnt;
	}
}

/* move cursor left one char */
void tac_key_left(tty_device_t *tty) {
	if (tty->rx_cur > 0) {
		--tty->rx_cur;
		/* vtbuild((struct vtbuild *)tty->vtb, token); */
		tty_go_left(tty, 1);
	}
}

/* move cursor right one char */
void tac_key_right(tty_device_t *tty) {
	if (tty->rx_cur < tty->rx_cnt) {
		++tty->rx_cur;
		/* vtbuild((struct vtbuild *)tty->vtb, token); */
		tty_go_right(tty, 1);
	}
}

/* move cursor to begin of the line */
void tac_key_home(tty_device_t *tty) {
	tty_go_cursor_position(cur_tty, cur_tty->rx_cur, 0);
	cur_tty->rx_cur = 0;
}

/* move cursor to end of the line */
void tac_key_end(tty_device_t *tty) {
	tty_go_cursor_position(cur_tty, cur_tty->rx_cur, cur_tty->rx_cnt);
	cur_tty->rx_cur = cur_tty->rx_cnt;
}

/* switch insert-replace mode */
void tac_key_ins(tty_device_t *tty) {
	cur_tty->ins_mod = !cur_tty->ins_mod;
}

/* execute command line */
void tac_key_enter(tty_device_t *tty, struct vt_token *token) {
	#if 1
	if (tty->out_busy) return;
	#else
	while (tty->out_busy);
	#endif
	/* add string to output buffer */
	memcpy((void*) tty->out_buff, (const void*)
		tty->rx_buff, tty->rx_cnt);
	tty->out_buff[tty->rx_cnt] = '\0';
	tty->rx_cnt = 0;
	tty->rx_cur = 0;
	tty->out_busy = true;
	vtbuild((struct vtbuild *)tty->vtb, token);
}

/* remove last word in command line */
void tac_remove_word(tty_device_t *tty) {
	/* find end of prev word */
	uint32_t tps; /* tps - to position space */
	tps = tty->rx_cur>0 ? tty->rx_cur-1 : 0;
	for (; tps>0 && isalpha(tty->rx_buff[tps]); --tps);
	for (; tps>0 && isspace(tty->rx_buff[tps]); --tps);
	if (tps > 0) {
		++tps;
	}

	copy_forward(
		(uint8_t *) &tty->rx_buff[tty->rx_cur],
		(uint8_t *) &tty->rx_buff[tps],
		tty->rx_cnt - tty->rx_cur + tps);

	tty_rewrite_line(tty,
		(uint8_t *) &tty->rx_buff[tps],
		tty->rx_cnt - tty->rx_cur,
		tty->rx_cnt - tps,
		tty->rx_cur - tps, 0);

	tty->rx_cnt -= tty->rx_cur - tps;
	tty->rx_cur = tps;
}

/* remove the line */
void tac_remove_line(tty_device_t *tty) {
	copy_forward(
		(uint8_t *) &tty->rx_buff[tty->rx_cur],
		(uint8_t *) &tty->rx_buff[0],
		tty->rx_cnt - tty->rx_cur);
	tty_rewrite_line(tty,
		(uint8_t *) &tty->rx_buff[0],
		tty->rx_cnt - tty->rx_cur,
		tty->rx_cnt, tty->rx_cur, 0);
	tty->rx_cnt -= tty->rx_cur;
	tty->rx_cur = 0;
}

#ifdef CONFIG_TTY_CONSOLE_COUNT
/* switch console */
void tac_switch_console(tty_device_t *tty, uint32_t c_after) {
	vconsole_deactivate(&tty->console[tty->console_cur]);
	tty->console_cur = c_after;
	#if 0
	tty_go_left(tty, 7); /* erase `TTY-X$' */
	printf("TTY-%d$ ", tty->console_cur + 1);
	#endif
	vconsole_activate(&tty->console[tty->console_cur]);
}
#endif


void tac_goto00(tty_device_t *tty ) {
	uint8_t i;
	for (i=0;i<100;++i) {
		vtbuild((struct vtbuild*)tty->vtb, TOKEN_UP);
	}
	for (i=0;i<100;++i) {
		vtbuild((struct vtbuild*)tty->vtb, TOKEN_LEFT);
	}
}

void tac_clear(tty_device_t *tty ) {
	uint8_t i;
	struct vt_token vt;
	tac_goto00( tty );
	//return;

	vt = char_token('\n');
	for (i=0;i<25;++i) {
		tty_write_space( tty, 80 );
		//tty->file_op->fwrite("\n", sizeof(char), 1, NULL);
		vtbuild((struct vtbuild*)tty->vtb, &vt);
	}
	tac_goto00( tty );
}
//----------------------------------------------FROM VCONSOLE--------------------------------
/* move this to tty_action */
void tty_vconsole_write_char( vconsole_t *vc, uint8_t *ch ) {
	if (*ch>32 && *ch<128) {
		vc->tty->file_op->fwrite(ch,sizeof(uint8_t),1,NULL); /* */
	} else {
		vc->tty->file_op->fwrite(" ",sizeof(uint8_t),1,NULL); /* */
	}
}

void tty_vconsole_loadline(vconsole_t *con) { // uint32_t ?!
	uint32_t *s = (uint32_t*) &con->tty->rx_cnt;
	uint32_t *t = (uint32_t*) &con->tty->rx_cur;
	/* write saved command line */
	*t = con->cl_cur;
	for (*s=0; *s<con->cl_cnt; ++*s) {
		//diag_putc(con->tty->rx_buff[*s] = con->cl_buff[*s]);
		con->tty->rx_buff[*s] = con->cl_buff[*s];
		con->tty->file_op->fwrite(&(con->tty->rx_buff[*s]),sizeof(char),1,NULL);
		//con->tty->file_op->fwrite()
	}
	/* go to saved cursor position */
	for (;*s>*t;--*s) {
		vtbuild((struct vtbuild *)con->tty->vtb, TOKEN_LEFT);
	}
	*s = con->cl_cnt;
}

void tty_vconsole_saveline(vconsole_t *con) {
	uint32_t *s, *t;
	s = (uint32_t*) &con->tty->rx_cnt;
	t = (uint32_t*) &con->tty->rx_cur;
	con->cl_cnt = *s;
	con->cl_cur = *t;
	/* clear current command line */
	for (;*t>0;--*t) {
		vtbuild((struct vtbuild*)con->tty->vtb, TOKEN_LEFT);
	}
	for (;*t<*s;++*t) {
		con->tty->file_op->fwrite(" ",sizeof(char),1,NULL);
	}
	/* copy command line in buffer */
	con->cl_buff[0] = con->tty->rx_buff[0];
	for (;*s>0;--*s) {
		con->cl_buff[*s] = con->tty->rx_buff[*s];
		vtbuild((struct vtbuild*)con->tty->vtb, TOKEN_LEFT);
	}
	*s = *t = 0;
}


void tty_vconsole_reprint(struct vconsole *vc);

int vconsole_activate(struct vconsole *con) {
	tty_vconsole_reprint(con); // tty_reprint()
	tty_vconsole_loadline(con);
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	tty_vconsole_saveline( con );
	return 0;
}

int vconsole_close(vconsole_t *con) {
	return 0;
}

#ifdef CONFIG_TTY_CONSOLE_COUNT
/* move to tty. here don't need to know about this */
static inline bool console_is_current(void) {
	struct thread *thread;
	if (NULL == (thread = thread_self())) {
		return false;
	}
	if (NULL == thread->task.own_console) {
		return false;
	}
	return (&(thread->task.own_console->tty->
			console[thread->task.own_console->tty->console_cur])
	          == thread->task.own_console);
}
#define CONSOLE_IS_CURRENT	console_is_current()
#define EXECUTE_IF_CONSOLE_IS_CURRENT(a) do { if (console_is_current()) a; } while (0)
#else
#define CONSOLE_IS_CURRENT	true
#define EXECUTE_IF_CONSOLE_IS_CURRENT(a) a
#endif

void tty_vconsole_putchar( struct vconsole *vc, char ch ) {
	bool reprint_all_console = false;
	if (vc==NULL) { /* if hasn't initialized now current_thread->own_console use hardware output */
		diag_putc( ch );
		return;
	}
	if (ch!='\n') {
		vconsole_putcharXY(vc, vc->scr_column, vc->scr_line, ch);
	} else {
		vc->scr_column = 0;
		++ vc->scr_line;
	}

	if (ch!='\n') {
		++ vc->scr_column;
		if (vc->scr_column>=vc->width) {
			vc->scr_column = 0;
			++ vc->scr_line;
		}
	}

	if (vc->scr_line>=vc->height) {
		reprint_all_console = true;
		vc->scr_line=vc->height-1;
		copy_forward(&vc->scr_buff[vc->width],&vc->scr_buff[0],sizeof(char)*vc->width*(vc->height-1));
		{	uint32_t i;
			for (i=vc->width*(vc->height-1); i<vc->width*vc->height; ++i) {
				vc->scr_buff[i] = ' ';
			}
		}
	}

	if (CONSOLE_IS_CURRENT) {
		if (reprint_all_console) {
			tty_vconsole_reprint(vc);
		} else {
			if (ch!='\n') {
				while (vc->tty->in_busy);
				//vc->tty->in_busy = true;

				tty_vconsole_write_char(vc,&ch);

				vc->tty->in_busy = false;
			} else {
				vc->tty->file_op->fwrite("\n",sizeof(char),1,NULL);
			}
		}
	}
}

/* move & refactoring this interface */
void tty_gotoXYvector(struct vconsole *vc, int32_t x, int32_t y) {
	tty_vconsole_saveline( vc );
	if (CONSOLE_IS_CURRENT) {
		uint8_t i;
		while (vc->tty->in_busy);
		//vc->tty->in_busy = true;

		if (x>0) {
			for (i=0;i<x;++i) {
				vtbuild((struct vtbuild*)vc->tty->vtb, TOKEN_RIGHT);
			}
		} else {
			for (i=0;i<(-x);++i) {
				vtbuild((struct vtbuild*)vc->tty->vtb, TOKEN_LEFT);
			}
		}
		if (y>0) {
			for (i=0;i<(y);++i) {
				vtbuild((struct vtbuild*)vc->tty->vtb, TOKEN_DOWN);
			}
		} else {
			for (i=0;i<(-y);++i) {
				vtbuild((struct vtbuild*)vc->tty->vtb, TOKEN_UP);
			}
		}

		vc->tty->in_busy = false;
	}
	/* it's vconsole_gotoxy */
	vc->scr_column += x;
	vc->scr_line += y;
	tty_vconsole_loadline( vc );
}

void console_gotoXY( uint8_t x, uint8_t y ) {
}

void tty_vconsole_gotoXY(struct vconsole *vc, uint8_t x, uint8_t y) {
	if (CONSOLE_IS_CURRENT) {
		while (vc->tty->in_busy);
		//vc->tty->in_busy = true;

		vconsole_gotoxy(vc,x,y);
		tac_goto00(vc->tty);
		tty_gotoXYvector(vc,x,y);

		vc->tty->in_busy = false;
	}
}



/* decouple this from tty_clear */
void console_clear(void) {
	struct thread *thread = thread_self();

	vconsole_clear(thread->task.own_console);
	if (CONSOLE_IS_CURRENT) {
		tty_vconsole_saveline(thread->task.own_console);
		console_reprint();
		tty_vconsole_loadline(thread->task.own_console);
	}
}

void tty_vconsole_reprint(struct vconsole *vc) {
	uint8_t i,j;
	if (vc==NULL) return;
	if (!CONSOLE_IS_CURRENT) return;
	while (vc->tty->in_busy);
	vc->tty->in_busy = true;

	tty_vconsole_saveline(vc);

	tac_goto00( vc->tty );
	//vconsole_reprint_nline( vc, vc->height - 1 );
	for (i = 0; i < (vc->height) ; ++i) {
		for (j = 0; j< (vc->width) ; ++j ) {
			tty_vconsole_write_char(vc, &vc->scr_buff[i*vc->width + j]);
		}
		vc->tty->file_op->fwrite("\n",sizeof(uint8_t),1,NULL); /* tty->next_line() */
	}
	tty_vconsole_gotoXY(vc,vc->scr_column,vc->scr_line);

	tty_vconsole_loadline(vc);
	vc->tty->in_busy = false;
}

void console_reprint(void) {
	struct thread *thread = thread_self();
	if (CONSOLE_IS_CURRENT) {
		tty_vconsole_reprint(thread->task.own_console);
	};
}

//------------------------------------------------------------------------------------------------------------------------
void console_putchar(char ch) {
	struct thread *thread = thread_self();
	tty_vconsole_putchar(thread->task.own_console, ch);
}

char console_getchar(void) {
	struct thread *thread = thread_self();
	return '\0';
	return tty_vconsole_getchar(thread->task.own_console);
}

