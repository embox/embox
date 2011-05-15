/**
 * @file
 * @brief Serves all tty devices
 *
 * @date 12.11.10
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <unistd.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <embox/unit.h>

#include <kernel/thread/api.h> /*we create some threads here*/
#include <kernel/timer.h> /* use it */
#include <kernel/evdispatch.h>

#include <drivers/vtbuild.h>
#include <drivers/vtparse.h>
#include <drivers/tty.h>
#include <drivers/tty_action.h>

#include <framework/cmd/api.h> /*we start default shell*/

EMBOX_UNIT_INIT(tty_init);

tty_device_t *cur_tty = NULL;

#define USE_DIRECTLY_EVDISPATCH 0
#define TTY_IRQ_ID 1

#ifndef CONFIG_TTY_CONSOLE_COUNT
#error define TTY_CONSOLE_COUNT in options-kernel.conf before try to build.
#endif

/* FIXME TTY is space there is in library */
#if 0
inline bool tty_isalpha(char ch) {
	return ch != ' ';
}

inline bool tty_isspace(char ch) {
	return ch == ' ';
}
#else
    #define tty_isalpha isalpha
    #define tty_isspace isspace
#endif

void debug_output( tty_device_t *tty ) {
	int i,j;
	prom_printf("\n-f12 pressed----debug info about tty---\n");
	prom_printf("current console: %d;",cur_tty->console_cur);
	#define CONS cur_tty->consoles[cur_tty->console_cur]
	if (! CONS ) {
		prom_printf("console isn't initialized.\n");
		return;
	}
	prom_printf(" width: %d, height: %d;",CONS->width, CONS->height);
	prom_printf(" column: %d, line: %d;\n",CONS->scr_column, CONS->scr_line);
	prom_printf("CONS->cl_cnt: %d; CONS->cl_buff: ",CONS->cl_cnt);
	for (i=0;i<CONS->cl_cnt;++i) {
		prom_printf("%c",CONS->cl_buff[i]);
	}
	prom_printf("\nCONS->scr_buff: ");
	for (i=0;i<CONS->width;++i)
	for (j=0;j<CONS->height;++j) {
		#define CONS_CHAR CONS->scr_buff[i*CONS->width+j]
		//prom_printf("%d:",CONS_CHAR);
		if (' '<CONS_CHAR && CONS_CHAR<128) {
			prom_printf("%c",CONS_CHAR);
		}
		prom_printf(";");
		#undef CONS_CHAR
	}
	prom_printf("\n");
	#undef CONS
}

void tty_vtparse_callback(struct vtparse *tty_vtparse, struct vt_token *token) {
#if 0
	//#warning USING DEBUG OUTPUT IN TTY DRIVER!!!

	size_t i;

	prom_printf("action: %d; char: %d %c; attrs_len: %d; params_len: %d\n",
		token->action, token->ch, token->ch,
		token->attrs_len, token->params_len);
	for (i = 0; i < token->attrs_len; ++i) {
		prom_printf("(char) attrs[%d] = %d : %c\n", i, token->attrs[i], token->attrs[i]);
	}
	for (i = 0; i < token->params_len; ++i) {
		prom_printf("(char) params[%d] = %d\n", i, token->params[i]);
	}
	#if 0
	return;
	#endif
#endif
	switch (token->action) {
	case VT_ACTION_PRINT: /* Print any char */
		tac_key_alpha(cur_tty, token);
		break;
	case VT_ACTION_ESC_DISPATCH:
		/* printk("test\n"); */
	break;
	case VT_ACTION_CSI_DISPATCH:
		switch (token->ch) {
		case 'D': /* LEFT */
			tac_key_left(cur_tty);
			break;
		case 'C': /* RIGHT */
			tac_key_right(cur_tty);
			break;
		case '~': /* HOME, END and others */
			if (token->params_len == 1) {
				switch (token->params[0]) {
				case 7: /* HOME */
					tac_key_home(cur_tty);
					break;
				case 8: /* END */
					tac_key_end(cur_tty);
					break;
				case 3: /* DEL */
					tac_key_del(cur_tty);
					break;
				case 2: /* INS */
					tac_key_ins(cur_tty);
					break;
				case 24: /* F12 // Debug output */
					debug_output(cur_tty);
					break;
				}
			}
			break;
		case '^': /* ^F1-^F12 switch console */
			if ((token->params_len == 1) &&
			    (token->params[0] - 10 <= CONFIG_TTY_CONSOLE_COUNT)) {
				tac_switch_console(cur_tty, token->params[0] - 11);
			}
			break;
		default:
			break;
		}
		break;
	case VT_ACTION_EXECUTE:
		switch (token->ch) {
		case '\n': /* ENTER key. Return string */
			//prom_printf("\nenter was pressed\n"); //remove me in the end of world
			tac_key_enter(cur_tty, token);
			break;
		case 21: /* ^U clean line */
			tac_remove_line(cur_tty);
			break;
		case 23: /* ^W remove last word */
			tac_remove_word(cur_tty);
			break;
		case 8: /* ^H equalent to backspace */
		case 127: /* backspace */
			tac_key_backspace(cur_tty);
			break;
		case 4: /* ^D */
			break;
		}
	default:
		//LOGERROR?
		break;
	}
}

void tty_vtbuild_callback(struct vtbuild *tty_vtbuild, char ch) {
	cur_tty->file_op->fwrite(&ch, sizeof(char), 1, NULL);
}

static void *run_shell(void *data) {
	const struct cmd *def_shell;
	struct vconsole *console;
	int console_number;
	struct thread *thread;
	console = (struct vconsole*)data;
	console_number = console->id;

	thread = thread_self();
	thread->task.own_console = console;

	console_clear();
	printf("Hello from TTY%d!\n\n",console_number); /* this is output to the i-th console */

	if (NULL == (def_shell = cmd_lookup(CONFIG_DEFAULT_SHELL))) {
		prom_printf(" ERROR: unfound shell named (%s)\n", CONFIG_DEFAULT_SHELL);
		return NULL;
	}
	def_shell->exec(0,NULL);
	return NULL;
}

extern void tty_vconsole_putchar( struct vconsole *con, uint8_t ch);
extern void tty_vconsole_putchar_cc( struct vconsole *con, uint8_t ch,bool cc_flag);

void timer_handler(uint32_t id) {
#if ENABLE_CONSOLE_QUEUE
	uint32_t cc; /* console in cycle */
#endif

#if USE_DIRECTLY_EVDISPATCH
	event_dispatch( TTY_IRQ_ID, NULL );
#else

	if (cur_tty->handle_events || !cur_tty->has_events) return;
	cur_tty->handle_events = true;

	/* some routine */
	while (!TTY_RX_QUEUE_EMPTY(cur_tty)) {
		uint8_t charvar;
		TTY_RX_QUEUE_POP(cur_tty,charvar);
		vtparse((struct vtparse *) cur_tty->vtp, charvar);
	}
	/* add some routine for PRINT like above */

#if ENABLE_CONSOLE_QUEUE
	for (cc=0;cc<CONFIG_TTY_CONSOLE_COUNT;++cc) {
		#define CON cur_tty->consoles[cc]
		#define CONSOLE_IS_CURRENT (cc == cur_tty->console_cur)

		while (!CONS_TX_QUEUE_EMPTY(CON)) { /* PANIC!!! init console only in new thread, but try handle before that */
			uint8_t charvar;
			CONS_TX_QUEUE_POP(CON,charvar);
			//tty_vconsole_putchar(CON,charvar);
			tty_vconsole_putchar_cc(CON,charvar,CONSOLE_IS_CURRENT);
			if (CONSOLE_IS_CURRENT) {
				cur_tty->file_op->fwrite(&charvar,sizeof(char),1,NULL);
			}
		}

		#undef CONSOLE_IS_CURRENT
		#undef CON
	}
#endif


	cur_tty->has_events = false;
	cur_tty->handle_events = false;
#endif
}

//TODO remove this
void add_char_handler(struct event_msg *ev) {
	vtparse((struct vtparse *) cur_tty->vtp, (char) ((long)ev->data-1000));
}

static int tty_init(void) {
	size_t i;
	static FILE *def_file;

	def_file = fopen(CONFIG_DEFAULT_CONSOLE, "r");

	if (NULL == cur_tty) {
		LOG_ERROR(" Any TTY has not registred!\n");
		return -1;
	}

	cur_tty->handle_events = true;
	cur_tty->has_events = false;

	TTY_RX_QUEUE_INIT(cur_tty);

	/* now we want to initialize first console
	 * it use current thread. we just add pointer to thread resources
	 */
	cur_tty->console_cur = 0;
	for (i = 0; i < CONFIG_TTY_CONSOLE_COUNT; ++i) {
		static int console_numbers[CONFIG_TTY_CONSOLE_COUNT];
		struct thread* new_thread;
		console_numbers[i] = i;

		vconsole_create(i, cur_tty);

		thread_create(&new_thread,
				THREAD_FLAG_PRIORITY_INHERIT | THREAD_FLAG_DETACHED,
				run_shell, (void*) cur_tty->consoles[i]);
	}

	cur_tty->console_cur = 0; /* foreground console by default */

	set_timer(TTY_IRQ_ID, 10, &timer_handler);
	event_register( TTY_IRQ_ID, &add_char_handler );

	cur_tty->handle_events = false;
	cur_tty->has_events = false;

	return 0;
}


int tty_register(tty_device_t *tty) {
	if (NULL == vtparse_init((struct vtparse *) tty->vtp, tty_vtparse_callback)) {
		LOG_ERROR("Error while initialization vtparse.\n");
	}
	if (NULL == vtbuild_init((struct vtbuild *) tty->vtb, tty_vtbuild_callback)) {
		LOG_ERROR("Error while initialization vtbuild.\n");
	}

	tty->rx_cur = 0;
	tty->rx_cnt = 0;
	tty->ins_mod = true; /* what must be default, don't know */

	cur_tty = tty;
	return 0;
}

int tty_unregister(tty_device_t *tty) {
	tty->out_busy = false;
	tty->rx_cnt = 0;
	cur_tty = tty; //TODO only if single console
	return 0;
}

int tty_get_uniq_number(void) {
	static int unic_id = 0;

	return unic_id++;
}


/**
 * add parsed char to receive buffer
 */
int tty_add_char(tty_device_t *tty, int ch) {
#if 1
#if USE_DIRECTLY_EVDISPATCH
	event_send( TTY_IRQ_ID, (void*)((long)ch + 1000) );
#else
	TTY_RX_QUEUE_PUSH(tty,ch);
	tty->has_events = true;
#endif
#else
	vtparse((struct vtparse *) cur_tty->vtp, ch);
#endif
	return 0;
}

uint8_t* tty_readline(tty_device_t *tty) {
	struct thread *thread = thread_self();
	//printf("%d %%",tty->consoles[tty->console_cur]->scr_line);
	//while ((!tty->consoles[tty->console_cur]->out_busy)
	while ((!thread->task.own_console->out_busy)){
	}
	thread->task.own_console->out_busy = false;
	return (uint8_t*) thread->task.own_console->out_buff;
}

static uint32_t tty_scanline(uint8_t line[TTY_RXBUFF_SIZE + 1], uint32_t size) {
	size_t i = 0;
	while ('\n' == line[i++]) {
		if (0 == (size--))
			return 0;
	}
	return (uint32_t) (i - 1);
}

void tty_freeline(tty_device_t *tty, uint8_t *line) {
	uint32_t line_size;
	if (0 != tty->rx_cnt) {
		line_size = tty_scanline((uint8_t*) tty->rx_buff, tty->rx_cnt);
	}
}
