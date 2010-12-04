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
#include <kernel/uart.h>
#include <ctype.h>

tty_device_t *cur_tty;
#ifdef CONFIG_TTY_SYSTEM_COUNT
vconsole_t tty_console[CONFIG_TTY_SYSTEM_COUNT];
uint32_t tty_console_cur;
#endif

struct vtparse tty_vtparse[1];
struct vtbuild tty_vtbuild[1];

#if 0
#define TTY_USE_CONTROL_H_CHAR
#endif

const struct vt_token TOKEN_LEFT[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'D'
}};

const struct vt_token TOKEN_RIGHT[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'C'
}};

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
		case VT_ACTION_PRINT:
			if (cur_tty->ins_mod) { /* INSERT MODE */
				if (cur_tty->rx_cnt < TTY_RXBUFF_SIZE) {
					uint32_t i;
					++cur_tty->rx_cnt;
					for (i=cur_tty->rx_cnt; i>cur_tty->rx_cur; --i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i-1];
					}
					cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
					vtbuild(tty_vtbuild, token);
					for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
						uart_putc(cur_tty->rx_buff[i]);
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
					}
					for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
						#ifdef TTY_USE_CONTROL_H_CHAR
						uart_putc(8);
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
					}
				}
			} else { /* REPLACE MOD */
				if (cur_tty->rx_cur < TTY_RXBUFF_SIZE) {
					cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
					if ( cur_tty->rx_cur > cur_tty->rx_cnt ) {
						cur_tty->rx_cnt = cur_tty->rx_cur;
					}
					vtbuild(tty_vtbuild, token);
				}
			}
		break;

		case VT_ACTION_CSI_DISPATCH:
			switch (token->ch) {
				case 'D': /* LEFT */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						vtbuild(tty_vtbuild, token);
					}
				break;
				case 'C': /* RIGHT */
					if (cur_tty->rx_cur < cur_tty->rx_cnt) {
						++cur_tty->rx_cur;
						vtbuild(tty_vtbuild, token);
					}
				break;
				case '~': /* HOME, END and others */
					if (token->params_len == 1) {
						switch (token->params[0]) {
							case 7: /* HOME */
								while (cur_tty->rx_cur>0) {
									--cur_tty->rx_cur;
									#ifdef TTY_USE_CONTROL_H_CHAR
									uart_putc(8);
									#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
									#else
									vtbuild(tty_vtbuild, TOKEN_LEFT);
									#endif
								}
							break;
							case 8: /* END */
								while (cur_tty->rx_cur<cur_tty->rx_cnt) {
									++cur_tty->rx_cur;
									vtbuild(tty_vtbuild, TOKEN_RIGHT);
								}
							break;
							case 3: /* DEL */
								if (cur_tty->rx_cur<cur_tty->rx_cnt) {
									uint32_t i;
									for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt-1; ++i) {
										cur_tty->rx_buff[i] = cur_tty->rx_buff[i+1];
										uart_putc(cur_tty->rx_buff[i]);
										#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
									}
									uart_putc(' ');
									#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
									for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
										#ifdef TTY_USE_CONTROL_H_CHAR
										uart_putc(8);
										#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
										#else
										vtbuild(tty_vtbuild, TOKEN_LEFT);
										#endif
									}
									--cur_tty->rx_cnt;
								}
							break;
							case 2: /* INS */
								cur_tty->ins_mod = !cur_tty->ins_mod;
							break;
						}
					}
				break;
				case '^': /* ^F1-^F12 switch console */
				#ifdef CONFIG_TTY_SYSTEM_COUNT
					if ((token->params_len==1) &&
						(token->params[0]-10<=CONFIG_TTY_SYSTEM_COUNT)) {
						#if 0
						printf("^F%d",token->params[0]-10);
						#endif
						vconsole_deactivate(&tty_console[tty_console_cur]);
						tty_console_cur = token->params[0]-11;
						uart_putc(8);
						uart_putc(8);
						uart_putc(8);
						uart_putc(8);
						uart_putc(8);
						uart_putc(8);
						uart_putc(8);
						printf("TTY-%d$ ",tty_console_cur+1);
						vconsole_activate(&tty_console[tty_console_cur]);
					}
				#endif
				break;
			}
		break;

		case VT_ACTION_EXECUTE:
			switch (token->ch) {
				case '\n':
					if (cur_tty->out_busy) break;
					/* add string to output buffer */
					memcpy((void*) cur_tty->out_buff,(const void*)
						cur_tty->rx_buff, cur_tty->rx_cnt);
					cur_tty->out_buff[cur_tty->rx_cnt] = '\0';
					cur_tty->rx_cnt = 0;
					cur_tty->rx_cur = 0;
					cur_tty->out_busy = true;
					vtbuild(tty_vtbuild, token);
				break;

				case 21: /* ^U clean line */ {
					uint32_t i;
					for (i=0; i<cur_tty->rx_cur; ++i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i+cur_tty->rx_cur];
					}
					for (i=cur_tty->rx_cur; i>0; --i) {
						#ifdef TTY_USE_CONTROL_H_CHAR
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(8);
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
					}
					cur_tty->rx_cnt -= cur_tty->rx_cur;
					for (i=0; i<cur_tty->rx_cnt; ++i) {
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(cur_tty->rx_buff[i]);
					}
					for (i=0; i<cur_tty->rx_cur; ++i) {
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(' ');
					}
					for (i=cur_tty->rx_cur+cur_tty->rx_cnt; i>0; --i) {
						#ifdef TTY_USE_CONTROL_H_CHAR
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(8);
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
					}
					cur_tty->rx_cur = 0;
				} break;

				case 23: /* ^W remove last word */ {
					uint32_t i,tps; /* tps - to position space */
					for (tps=cur_tty->rx_cur; tps>0 && tty_isalpha(cur_tty->rx_buff[tps]); --tps);
					for (; tps>0 && tty_isspace(cur_tty->rx_buff[tps]); --tps);
					if (tps>0) {++tps;}
					//printf("\ntps: %d\n",tps);return;

					for (i=tps; i<cur_tty->rx_cur; ++i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i+cur_tty->rx_cur-tps];
					}
					for (i=cur_tty->rx_cur; i>tps; --i) {
						#ifdef TTY_USE_CONTROL_H_CHAR
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(8);
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
					}
					cur_tty->rx_cnt -= cur_tty->rx_cur-tps;
					for (i=tps; i<cur_tty->rx_cnt; ++i) {
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(cur_tty->rx_buff[i]);
					}
					for (i=tps; i<cur_tty->rx_cur; ++i) {
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(' ');
					}
					for (i=cur_tty->rx_cnt+cur_tty->rx_cur-tps; i>tps; --i) {
						#ifdef TTY_USE_CONTROL_H_CHAR
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						uart_putc(8);
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
					}
					cur_tty->rx_cur = tps;
				}
				break;

				case 8: /* ^H equalent to backspace */
				case 127: /* backspace */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						#ifdef TTY_USE_CONTROL_H_CHAR
						uart_putc(8);
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						#else
						vtbuild(tty_vtbuild, TOKEN_LEFT);
						#endif
						uint32_t i;
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt-1; ++i) {
							cur_tty->rx_buff[i] = cur_tty->rx_buff[i+1];
							uart_putc(cur_tty->rx_buff[i]);
							#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						}
						uart_putc(' ');
						#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
							#ifdef TTY_USE_CONTROL_H_CHAR
							#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
							uart_putc(8);
							#else
							vtbuild(tty_vtbuild, TOKEN_LEFT);
							#endif
						}
						--cur_tty->rx_cnt;
					}
				break;

				case 4: /* ^D */
				break;

				default:
				break;
			}
		default:
		break;
	}
}

void tty_vtbuild_callback(struct vtparse *tty_vtbuild, char ch) {
	#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
	uart_putc(ch);
}

static int tty_init_flag = 0;

int tty_init(void) {
	if (NULL == vtparse_init(tty_vtparse, tty_vtparse_callback)) {
		LOG_ERROR("Error while initialization vtparse.\n");
	}
	if (NULL == vtbuild_init(tty_vtbuild, tty_vtbuild_callback)) {
		LOG_ERROR("Error while initialization vtbuild.\n");
	}
	cur_tty->rx_cur = 0;
	cur_tty->rx_cnt = 0;
	cur_tty->ins_mod = true;	/* what must be default, don't know */

#ifdef CONFIG_TTY_SYSTEM_COUNT
	tty_console_cur = 0;
	uint32_t i;
	for (i=0; i<CONFIG_TTY_SYSTEM_COUNT; ++i) {
		tty_console[i].tty = cur_tty;
	}
#endif

	return 0;
}

int tty_register(tty_device_t *tty) {
	cur_tty = tty;
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
	if (!tty_init_flag) {
		tty_init_flag = 1;
		tty_init();
	}
	vtparse(tty_vtparse, ch);
	return 0;
}

uint8_t* tty_readline(tty_device_t *tty) {
	#ifdef CONFIG_TTY_SYSTEM_COUNT
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


