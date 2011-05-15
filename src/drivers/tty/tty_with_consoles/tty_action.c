/**
 * @file
 *
 * @date 07.12.10
 * @author Fedor Burdun
 */

#include <unistd.h>
#include <kernel/thread/api.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <drivers/tty.h>
#include <drivers/tty_action.h>
#include <kernel/diag.h>

void tac_switch_console(tty_device_t *tty, uint32_t c_after) {
	//prom_printf("switch to: %d\n",c_after);
	vconsole_deactivate(tty->consoles[tty->console_cur]);
	tty->console_cur = c_after;
	tty->file_op->fwrite("\x1b[2J",sizeof(char),4,NULL); /* tac_clear */
	#if 0
	tty_go_left(tty, 7); /* erase `TTY-X$' */
	printf("TTY-%d$ ", tty->console_cur + 1);
	#endif
	vconsole_activate(tty->consoles[tty->console_cur]);
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
	uint32_t s = con->cl_cnt;
	uint32_t t = con->cl_cur;
	uint8_t *buff = (uint8_t*) &con->cl_buff[0];

	//con->tty->file_op->fwrite("\x1b[H",sizeof(char),3,NULL); //goto 0,0

	/* write saved command line */
	for (s=0; s<con->cl_cnt; ++s) {
		//prom_printf("char: %c:%d;",buff[s],buff[s]);
		con->tty->file_op->fwrite(&(buff[s]),sizeof(char),1,NULL);
	}
	/* go to saved cursor position */
	for (;s>t;--s) {
		vtbuild((struct vtbuild *) con->tty->vtb, TOKEN_LEFT);
	}
}

/* TODO remove */
void tty_clearline(struct tty_device *tty) {
	uint32_t *s, *t;
	#define con tty->consoles[tty->console_cur]
	s = (uint32_t*) &con->cl_cnt;
	t = (uint32_t*) &con->cl_cur;
	con->cl_cnt = *s;
	con->cl_cur = *t;
	/* clear current command line */
	for (;*t>0;--*t) {
		vtbuild((struct vtbuild*) con->tty->vtb, TOKEN_LEFT);
	}
	for (;*t<*s;++*t) {
		con->tty->file_op->fwrite(" ",sizeof(char),1,NULL);
	}
	#undef con
}


void tty_vconsole_reprint(struct vconsole *vc);
void tty_reprint(struct tty_device *tty);

int vconsole_activate(struct vconsole *con) {
	if (NULL==con) return 0;
	//tty_vconsole_reprint(con); // tty_reprint()
	tty_reprint(con->tty);
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	if (NULL==con) return 0;
	//tty_vconsole_saveline( con );
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
	return ((thread->task.own_console->tty->
			consoles[thread->task.own_console->tty->console_cur])
	          == thread->task.own_console);
}
//#ifdef ENABLE_CONSOLE_QUEUE
//#define CONSOLE_IS_CURRENT cc_flag
//#else
#define CONSOLE_IS_CURRENT	console_is_current()
//#endif
#define EXECUTE_IF_CONSOLE_IS_CURRENT(a) do { if (console_is_current()) a; } while (0)
#else
#define CONSOLE_IS_CURRENT	true
#define EXECUTE_IF_CONSOLE_IS_CURRENT(a) a
#endif

char tty_vconsole_getchar( struct vconsole *vc ) {
	while (true) ;
	return '\0';
}

void tty_vconsole_putchar( struct vconsole *vc, char ch ) {
	bool reprint_all_console = false;
	if (vc==NULL) { /* if hasn't initialized now current_thread->own_console use hardware output */
		//diag_putc( ch );
		return;
	}

	if (ch!='\n') {
		vconsole_putcharXY(vc, vc->scr_column++, vc->scr_line, ch);
	} else {
		vc->scr_column = 0;
		++ vc->scr_line;
	}
#if 1
	if (vc->scr_column>=vc->width) {
		vc->scr_column = 0;
		++ vc->scr_line;
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
	if (!CONSOLE_IS_CURRENT) return; /* don't work! */
	if (reprint_all_console) {
		tty_reprint(vc->tty);
	}
#endif
}

void tty_vconsole_putchar_cc( struct vconsole *vc, char ch, bool cc_flag ) {
	bool reprint_all_console = false;
	if (vc==NULL) { /* if hasn't initialized now current_thread->own_console use hardware output */
		//diag_putc( ch );
		return;
	}

	if (ch!='\n') {
		vconsole_putcharXY(vc, vc->scr_column++, vc->scr_line, ch);
	} else {
		vc->scr_column = 0;
		++ vc->scr_line;
	}
#if 1
	if (vc->scr_column>=vc->width) {
		vc->scr_column = 0;
		++ vc->scr_line;
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
	if (!cc_flag) return; /* don't work! */
	if (reprint_all_console) {
		tty_reprint(vc->tty);
	}
#endif
}

void tty_gotoXY(tty_device_t *tty, int32_t x, int32_t y) {
	if (1) {
		int8_t i;

		tac_goto00(tty);

		if (x>0) {
			for (i=0;i<x;++i) {
				vtbuild((struct vtbuild*)tty->vtb, TOKEN_RIGHT);
			}
		} else {
			for (i=0;i<(-x);++i) {
				vtbuild((struct vtbuild*)tty->vtb, TOKEN_LEFT);
			}
		}
		if (y>0) {
			for (i=0;i<(y);++i) {
				vtbuild((struct vtbuild*)tty->vtb, TOKEN_DOWN);
			}
		} else {
			for (i=0;i<(-y);++i) {
				vtbuild((struct vtbuild*)tty->vtb, TOKEN_UP);
			}
		}
	}
	/* it's vconsole_gotoxy */
	#define vc cur_tty->consoles[cur_tty->console_cur]
	//tty_vconsole_loadline( vc );
	#undef vc
}

/* move & refactoring this interface */
void tty_gotoXYvector(struct vconsole *vc, int32_t x, int32_t y) {
	//tty_vconsole_saveline( vc );
	if (CONSOLE_IS_CURRENT) {
		uint8_t i;

		prom_printf("gotoXY: %d %d\n",x,y);

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
	/*
	vc->scr_column += x;
	vc->scr_line += y;
	*/
	//tty_vconsole_loadline( vc );
}

void console_gotoXY( uint8_t x, uint8_t y ) {
}

void tty_vconsole_gotoXY(struct vconsole *vc, uint8_t x, uint8_t y) {
	if (CONSOLE_IS_CURRENT) {
		//while (vc->tty->in_busy);
		//vc->tty->in_busy = true;

		vconsole_gotoxy(vc,x,y);
		tac_goto00(vc->tty);
		tty_gotoXYvector(vc,x,y);

		//vc->tty->in_busy = false;
	}
}


/* decouple this from tty_clear */
void console_clear(void) {
	struct thread *thread = thread_self();
	//return;

	vconsole_clear(thread->task.own_console);
	if (CONSOLE_IS_CURRENT) {
		//tty_vconsole_saveline(thread->task.own_console);
		console_reprint();
		tty_vconsole_loadline(thread->task.own_console);
	}
}

void tty_reprint(struct tty_device *tty) {
	uint8_t i,j;
	#define vc cur_tty->consoles[cur_tty->console_cur]
	//tty_vconsole_saveline(vc);
	//vc->tty->file_op->fwrite("\x1b[2J",sizeof(char),4,NULL); /* tac_clear */
	tac_goto00( vc->tty );
	for (i = 0; i < (vc->height) ; ++i) {
		for (j = 0; j< (vc->width) ; ++j ) {
			tty_vconsole_write_char(vc, &vc->scr_buff[i*vc->width + j]);
		}
		vc->tty->file_op->fwrite("\n",sizeof(uint8_t),1,NULL); /* tty->next_line() */
	}
	tty_gotoXY( vc->tty,vc->scr_column,vc->scr_line);
	tty_vconsole_loadline(vc);
	#undef vc
}

void tty_vconsole_reprint(struct vconsole *vc) {
	uint8_t i,j;
	//return;
	if (vc==NULL) return;
	if (!CONSOLE_IS_CURRENT) return;
	//while (vc->tty->in_busy);
	//vc->tty->in_busy = true;
	//tty_vconsole_saveline(vc);
	vc->tty->file_op->fwrite("\x1b[2J",sizeof(char),4,NULL); /* tac_clear */ // it realy need?
	tac_goto00( vc->tty );
	//vconsole_reprint_nline( vc, vc->height - 1 );
	for (i = 0; i < (vc->height) ; ++i) {
		for (j = 0; j< (vc->width) ; ++j ) {
			tty_vconsole_write_char(vc, &vc->scr_buff[i*vc->width + j]);
		}
		vc->tty->file_op->fwrite("\n",sizeof(uint8_t),1,NULL); /* tty->next_line() */
	}
	tac_goto00( vc->tty );
	//tty_vconsole_gotoXY(vc,vc->scr_column,vc->scr_line);
	//tty_vconsole_loadline(vc);
	//vc->tty->in_busy = false;
}

void console_reprint(void) {
	struct thread *thread = thread_self();
	if (CONSOLE_IS_CURRENT) {
		tty_vconsole_reprint(thread->task.own_console);
	};
}

//------------------------------------------------------------------------------------------------------------------------
void console_putchar(char ch) {
#if ENABLE_CONSOLE_QUEUE
	CONS_TX_QUEUE_PUSH(thread_self()->task.own_console,ch);
#else
	/* TODO only CONS_TX_QUEUE_PUSH */
	if (CONSOLE_IS_CURRENT) {
		thread_self()->task.own_console->tty->file_op->fwrite(&ch,sizeof(char),1,NULL);
	}
	tty_vconsole_putchar(thread_self()->task.own_console, ch);
#endif
}

char console_getchar(void) {
	struct thread *thread = thread_self();
	return '\0';
	return tty_vconsole_getchar(thread->task.own_console);
}
