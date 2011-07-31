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

//----------------------------------------------FROM VCONSOLE--------------------------------
/* move this to tty_action */
void tty_vconsole_write_char(vconsole_t *vc, uint8_t *ch) {
	if (' ' < *ch && *ch < 128) {
		vc->tty->file_op->fwrite(ch, sizeof(char), 1, NULL); /* */
	} else {
		vc->tty->file_op->fwrite(" ", sizeof(char), 1, NULL); /* */
	}
}

void tty_vconsole_loadline(vconsole_t *con) { // uint32_t ?!
	uint32_t *p_cnt = (uint32_t*) &con->tty->rx_cnt;
	uint32_t *p_cur = (uint32_t*) &con->tty->rx_cur;
	/* write saved command line */
	*p_cur = con->cl_cur;
	for (*p_cnt = 0; *p_cnt < con->cl_cnt; ++*p_cnt) {
		//diag_putc(con->tty->rx_buff[*s] = con->cl_buff[*s]);
		con->tty->rx_buff[*p_cnt] = con->cl_buff[*p_cnt];
		con->tty->file_op->fwrite(con->tty->rx_buff + *p_cnt, sizeof(char), 1,
				NULL);
		//con->tty->file_op->fwrite()
	}
	/* go to saved cursor position */
	for (; *p_cnt > *p_cur; --*p_cnt) {
		vtbuild((struct vtbuild *) con->tty->vtb, TOKEN_LEFT);
	}
	*p_cnt = con->cl_cnt;
}

void tty_vconsole_saveline(vconsole_t *con) {
	uint32_t *s, *t;
	s = (uint32_t*) &con->tty->rx_cnt;
	t = (uint32_t*) &con->tty->rx_cur;
	con->cl_cnt = *s;
	con->cl_cur = *t;
	/* clear current command line */
	for (;*t>0;--*t) {
		vtbuild((struct vtbuild*) con->tty->vtb, TOKEN_LEFT);
	}
	for (;*t<*s;++*t) {
		con->tty->file_op->fwrite(" ",sizeof(char),1,NULL);
	}
	/* copy command line in buffer */
	con->cl_buff[0] = con->tty->rx_buff[0];
	for (;*s>0;--*s) {
		con->cl_buff[*s] = con->tty->rx_buff[*s];
		vtbuild((struct vtbuild*) con->tty->vtb, TOKEN_LEFT);
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

#define CONSOLE_IS_CURRENT	true
#define EXECUTE_IF_CONSOLE_IS_CURRENT(a) a

void tty_vconsole_putchar( struct vconsole *vc, char ch ) {
	return;
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
				tty_vconsole_write_char(vc, (uint8_t *) &ch);
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
		if (x>0) {
			for (i=0;i<x;++i) {
				vtbuild((struct vtbuild*) vc->tty->vtb, TOKEN_RIGHT);
			}
		} else {
			for (i=0;i<(-x);++i) {
				vtbuild((struct vtbuild*) vc->tty->vtb, TOKEN_LEFT);
			}
		}
		if (y>0) {
			for (i=0;i<(y);++i) {
				vtbuild((struct vtbuild*) vc->tty->vtb, TOKEN_DOWN);
			}
		} else {
			for (i=0;i<(-y);++i) {
				vtbuild((struct vtbuild*) vc->tty->vtb, TOKEN_UP);
			}
		}
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
		vconsole_gotoxy(vc,x,y);
		tac_goto00(vc->tty);
		tty_gotoXYvector(vc,x,y);
	}
}


/* decouple this from tty_clear */
void console_clear(void) {
	struct thread *thread = thread_self();
	return;

	vconsole_clear(thread->task.own_console);
	if (CONSOLE_IS_CURRENT) {
		tty_vconsole_saveline(thread->task.own_console);
		console_reprint();
		tty_vconsole_loadline(thread->task.own_console);
	}
}

void tty_vconsole_reprint(struct vconsole *vc) {
	uint8_t i,j;
	return;
	if (vc==NULL) return;
	if (!CONSOLE_IS_CURRENT) return;
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
	if (CONSOLE_IS_CURRENT) {
		diag_putc(ch);
	}
	tty_vconsole_putchar(thread->task.own_console, ch);
}

char console_getchar(void) {
//	struct thread *thread = thread_self();
	return '\0';
//	return tty_vconsole_getchar(thread->task.own_console);
}
