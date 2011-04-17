/**
 * @file
 * @brief Serves all tty devices
 *
 * @date 12.11.2010
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <types.h>
#include <ctype.h>
#include <string.h>
#include <embox/unit.h>

#include <kernel/thread/api.h> /*we create some threads here*/

#include <drivers/tty_action.h>
#include <drivers/vtbuild.h>
#include <drivers/vtparse.h>
#include <drivers/tty.h>

#include <framework/cmd/api.h> /*we start default shell*/

EMBOX_UNIT_INIT(tty_init);

tty_device_t *cur_tty = NULL;

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

void tty_vtparse_callback(struct vtparse *tty_vtparse, struct vt_token *token) {
#if 0
	#warning USING DEBUG OUTPUT IN TTY DRIVER!!!

	prom_printf("action: %d; char: %d %c; attrs_len: %d; params_len: %d\n",
		token->action, token->ch, token->ch,
		token->attrs_len, token->params_len);
	size_t i;
	for (i = 0; i < token->attrs_len; ++i) {
		prom_printf("(char) attrs[%d] = %d : %c\n", i, token->attrs[i]);
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
				}
			}
			break;
		case '^': /* ^F1-^F12 switch console */
			#ifdef CONFIG_TTY_CONSOLE_COUNT
			if ((token->params_len == 1) &&
			    (token->params[0] - 10 <= CONFIG_TTY_CONSOLE_COUNT)) {
				tac_switch_console(cur_tty, token->params[0] - 11);
			}
			#endif
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
	console_number = (int)data;
	console = vconsole_create(console_number, cur_tty);

	thread = thread_self();
	thread->task.own_console = console;

#if 0
	console_clear();
#endif
	printf("Hello from TTY%d!\n\n",console_number); /* this is output to the i-th console */

	if (NULL == (def_shell = cmd_lookup(CONFIG_DEFAULT_SHELL))) {
		prom_printf(" ERROR: unfound shell named (%s)\n", CONFIG_DEFAULT_SHELL);
		return NULL;
	}
	def_shell->exec(0,NULL);
	return NULL;
}


static int tty_init(void) {
#ifdef CONFIG_TTY_CONSOLE_COUNT
	size_t i;
	struct thread *thread = thread_self();
#endif
	static FILE *def_file;

	def_file = fopen(CONFIG_DEFAULT_CONSOLE, "r");

	if (NULL == cur_tty) {
		LOG_ERROR(" Any TTY has not registred!\n");
		return -1;
	}

#ifdef CONFIG_TTY_CONSOLE_COUNT
#if 0
	/* add clear screen */
	tac_clear( cur_tty );
#endif
	/* now we want to initialize first console
	 * it use current thread. we just add pointer to thread resources
	 */
	cur_tty->console_cur = 0;
	for (i = 1; i < CONFIG_TTY_CONSOLE_COUNT; ++i) {
#if 0
		static int console_numbers[CONFIG_TTY_CONSOLE_COUNT];
		struct thread* new_thread;
		console_numbers[i] = i;
		thread_create(&new_thread, 0, run_shell, (void*)console_numbers[i]);
		thread_change_priority(new_thread, thread->priority);
#else
		static uint8_t stacks[CONFIG_TTY_CONSOLE_COUNT][0x1000];
		struct thread* new_thread = thread_alloc();
		thread_init(new_thread, run_shell, &stacks[i], 0x1000);
		new_thread->priority = thread->priority;
		thread_start(new_thread);
#endif
	}

#if 0
	console_reprint();
#endif
#endif
	run_shell(0);
	cur_tty->console_cur = 0;

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

	return unic_id ++;
}




/**
 * add parsed char to receive buffer
 */
int tty_add_char(tty_device_t *tty, int ch) {
	vtparse((struct vtparse *) cur_tty->vtp, ch);
	return 0;
}

uint8_t* tty_readline(tty_device_t *tty) {
	struct thread *thread = thread_self();
	printf("%d %%",tty->console[tty->console_cur].scr_line);
	#ifdef CONFIG_TTY_CONSOLE_COUNT
	while ((!tty->out_busy)	||
			(thread->task.own_console != &tty->console[tty->console_cur])) {
	}
	#else
	while (!tty->out_busy);
	#endif
	++thread->task.own_console->scr_line; /* after read_line in console will be next line*/
	tty->out_busy = false;
	return (uint8_t*) tty->out_buff;
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

