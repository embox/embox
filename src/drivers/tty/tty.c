/**
 * @file
 *
 * @brief Serves all tty devices
 *
 * @date 12.11.2010
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <string.h>
#include <drivers/tty.h>
#include <drivers/vconsole.h>
#include <drivers/tty_action.h>
#include <kernel/uart.h>
#include <ctype.h>
#include <kernel/pp.h>
#include <embox/unit.h>

tty_device_t *cur_tty = NULL;

#ifndef CONFIG_ESH
#define CONFIG_ESH esh_run
#endif

extern void CONFIG_ESH (void) ;

#if 1
inline bool tty_isalpha(char ch) {
	return ch!=' ';
}

inline bool tty_isspace(char ch) {
	return ch==' ';
}
#else
#define tty_isalpha isalpha
#define tty_isspace isspace
#endif

void tty_vtparse_callback(struct vtparse *tty_vtparse, struct vt_token *token) {
#if 0
	#warning USING DEBUG OUTPUT IN TTY DRIVER!!!

	printf("action: %d; char: %d %c; attrs_len: %d; params_len: %d\n", token->action , token->ch, token->ch,
		token->attrs_len, token->params_len);
	uint32_t i;
	for ( i=0; i<token->attrs_len; ++i ) {
		printf("(char) attrs[%d] = %d : %c\n",i,token->attrs[i]);
	}
	for ( i=0; i<token->params_len; ++i ) {
		printf("(char) params[%d] = %d\n",i,token->params[i]);
	}
#endif

	switch (token->action) {
		case VT_ACTION_PRINT: /* Print any char */
			tac_key_alpha( cur_tty, token );
		break;

		case VT_ACTION_CSI_DISPATCH:
			switch (token->ch) {
				case 'D': /* LEFT */
					tac_key_left( cur_tty );
				break;
				case 'C': /* RIGHT */
					tac_key_right( cur_tty );
				break;
				case '~': /* HOME, END and others */
					if (token->params_len == 1) {
						switch (token->params[0]) {
							case 7: /* HOME */
								tac_key_home( cur_tty );
							break;
							case 8: /* END */
								tac_key_end( cur_tty );
							break;
							case 3: /* DEL */
								tac_key_del( cur_tty );
							break;
							case 2: /* INS */
								tac_key_ins( cur_tty );
							break;
						}
					}
				break;
				case '^': /* ^F1-^F12 switch console */
				#ifdef CONFIG_TTY_CONSOLE_COUNT
					if ((token->params_len==1) &&
						(token->params[0]-10<=CONFIG_TTY_CONSOLE_COUNT)) {
						tac_switch_console( cur_tty, token->params[0]-11 );
					}
				#endif
				break;
			}
		break;

		case VT_ACTION_EXECUTE:
			switch (token->ch) {
				case '\n': /* ENTER key. Return string */
					tac_key_enter( cur_tty, token );
				break;

				case 21: /* ^U clean line */
					tac_remove_line( cur_tty );
				break;

				case 23: /* ^W remove last word */
					tac_remove_word( cur_tty );
				break;

				case 8: /* ^H equalent to backspace */
				case 127: /* backspace */
					tac_key_backspace( cur_tty );
				break;

				case 4: /* ^D */
				break;

			}
	}
}

void tty_vtbuild_callback(struct vtparse *tty_vtbuild, char ch) {
	cur_tty->file_op->fwrite(&ch,sizeof(char),1,NULL);
}


static FILE *def_file;
static int tty_init_flag = 0;

static struct vconsole *cons;
static int cons_num;

void run_shell() {
#if 0
	printf("printf: running_shell\n");
	printk("printk: running_shell\n");
#endif
	CONFIG_ESH ();
}

static int tty_init(void) {
	printk("TTY_INIT: ");

	tty_init_flag = 1;

	def_file = fopen(CONFIG_DEFAULT_CONSOLE,"r");

	if (NULL == cur_tty) {
		printk(" [ error ]\n ");
		LOG_ERROR(" Any TTY has not registred!\n");
		return -1;
	}

	if (NULL == vtparse_init(cur_tty->vtp, tty_vtparse_callback)) {
		LOG_ERROR("Error while initialization vtparse.\n");
	}
	if (NULL == vtbuild_init(cur_tty->vtb, tty_vtbuild_callback)) {
		LOG_ERROR("Error while initialization vtbuild.\n");
	}

	printk(".");

	cur_tty->rx_cur = 0;
	cur_tty->rx_cnt = 0;
	cur_tty->ins_mod = true;	/* what must be default, don't know */

	printk(".");

#ifdef CONFIG_TTY_CONSOLE_COUNT
	scheduler_start();
	printk(".");

	cur_tty->console_cur = -1;
	uint32_t i;
	for (i=0; i<CONFIG_TTY_CONSOLE_COUNT; ++i) {
		printk(".");
		cons_num = i;
		cons = &cur_tty->console[i];
		cur_console = cons;
		cons->tty = cur_tty;
		cons->width = 80;
		cons->height = 25;
		vconsole_clear( cons );
		#if 0
		printf("Hello from TTY%d!\n",i+1);
		#endif
		pp_create_ws( run_shell, cons->esh_stack + CONFIG_ESH_STACK_S );
		printk(".");
	}
	cur_console = &cur_tty->console[0];
	cur_tty->console_cur = 0;
	printk(".");
#endif

	printk(" [ done ]\n");
	return 0;
}

int tty_register(tty_device_t *tty) {
	//printk("TTY was registred\n");
	cur_tty = tty;
	//threads_init(); // need before next
	//tty_init();
	return 0;
}

int tty_unregister(tty_device_t *tty) {
	tty->out_busy = false;
	tty->rx_cnt = 0;
	cur_tty = tty;
	return 0;
}

int tty_get_uniq_number(void) {
	return 0;
}

/*
 * add parsed char to receive buffer
 */
int tty_add_char(tty_device_t *tty, int ch) {

	vtparse(cur_tty->vtp, ch);
	return 0;
}

uint8_t* tty_readline(tty_device_t *tty) {
	#ifdef CONFIG_TTY_CONSOLE_COUNT
	printf("TTY-%d$ ",1);
	#endif
	while (!tty->out_busy);
	tty->out_busy = false;
	return (uint8_t*) tty->out_buff;
}

static uint32_t tty_scanline(uint8_t line[TTY_RXBUFF_SIZE + 1], uint32_t size) {
	int i = 0;
	while ('\n' == line[i++]) {
		if (0 == (size --))
			return 0;
	}
	return (uint32_t)(i - 1);
}

void tty_freeline(tty_device_t *tty, uint8_t *line) {
	uint32_t line_size;
	if (0 != tty->rx_cnt) {
		line_size = tty_scanline((uint8_t*) tty->rx_buff, tty->rx_cnt);
	}
}

int tty_e(void) {
	return 0;
}

//EMBOX_UNIT(tty_init,tty_e);
EMBOX_UNIT_INIT(tty_init);


