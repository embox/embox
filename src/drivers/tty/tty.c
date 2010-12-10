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

tty_device_t *cur_tty;
#ifdef CONFIG_TTY_SYSTEM_COUNT
vconsole_t tty_console[CONFIG_TTY_SYSTEM_COUNT];
uint32_t tty_console_cur;
#endif

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
			if (cur_tty->ins_mod) { /* INSERT MODE */
				if (cur_tty->rx_cnt < TTY_RXBUFF_SIZE) {
					#if 0
					uint32_t i;
					++cur_tty->rx_cnt;
					for (i=cur_tty->rx_cnt; i>cur_tty->rx_cur; --i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i-1];
					}
					cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
					vtbuild(cur_tty->vtb, token);
					for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
						cur_tty->file_op->fwrite(&cur_tty->rx_buff[i],sizeof(char),1,NULL);
					}
					for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
					}
					#else
					++cur_tty->rx_cnt;
					copy_backward(
						&cur_tty->rx_buff[cur_tty->rx_cur],
						&cur_tty->rx_buff[cur_tty->rx_cur+1],
						cur_tty->rx_cnt - cur_tty->rx_cur);
					cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
					vtbuild( cur_tty->vtb, token );
					tty_write_line( cur_tty, &cur_tty->rx_buff[cur_tty->rx_cur],
						cur_tty->rx_cnt - cur_tty->rx_cur, 0);
					#endif
				}
			} else { /* REPLACE MOD */
				if (cur_tty->rx_cur < TTY_RXBUFF_SIZE) {
					cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
					if ( cur_tty->rx_cur > cur_tty->rx_cnt ) {
						cur_tty->rx_cnt = cur_tty->rx_cur;
					}
					vtbuild(cur_tty->vtb, token);
				}
			}
		break;

		case VT_ACTION_CSI_DISPATCH:
			switch (token->ch) {
				case 'D': /* LEFT */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						//vtbuild(cur_tty->vtb, token);
						tty_go_left( cur_tty, 1 );
					}
				break;
				case 'C': /* RIGHT */
					if (cur_tty->rx_cur < cur_tty->rx_cnt) {
						++cur_tty->rx_cur;
						//vtbuild(cur_tty->vtb, token);
						tty_go_right( cur_tty, 1 );
					}
				break;
				case '~': /* HOME, END and others */
					if (token->params_len == 1) {
						switch (token->params[0]) {
							case 7: /* HOME */
								#if 0
								while (cur_tty->rx_cur>0) {
									--cur_tty->rx_cur;
									vtbuild(cur_tty->vtb, TOKEN_LEFT);
								}
								#else
								tty_go_cursor_position( cur_tty, cur_tty->rx_cur, 0 );
								cur_tty->rx_cur = 0;
								#endif
							break;
							case 8: /* END */
								#if 0
								while (cur_tty->rx_cur<cur_tty->rx_cnt) {
									++cur_tty->rx_cur;
									vtbuild(cur_tty->vtb, TOKEN_RIGHT);
								}
								#else
								tty_go_cursor_position( cur_tty, cur_tty->rx_cur, cur_tty->rx_cnt );
								cur_tty->rx_cur = cur_tty->rx_cnt;
								#endif
							break;
							case 3: /* DEL */
								if (cur_tty->rx_cur<cur_tty->rx_cnt) {
								#if 0
									uint32_t i;
									for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt-1; ++i) {
										cur_tty->rx_buff[i] = cur_tty->rx_buff[i+1];
										cur_tty->file_op->fwrite(&cur_tty->rx_buff[i],sizeof(char),1,NULL);
									}
									cur_tty->file_op->fwrite(" ",sizeof(char),1,NULL);
									for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
										vtbuild(cur_tty->vtb, TOKEN_LEFT);
									}
									--cur_tty->rx_cnt;
								#else
									copy_forward(
										&cur_tty->rx_buff[cur_tty->rx_cur+1],
										&cur_tty->rx_buff[cur_tty->rx_cur],
										--cur_tty->rx_cnt - cur_tty->rx_cur );
									tty_rewrite_line( cur_tty,
										&cur_tty->rx_buff[cur_tty->rx_cur],
										cur_tty->rx_cnt - cur_tty->rx_cur,
										cur_tty->rx_cnt - cur_tty->rx_cur + 1, 0, 0);
								#endif
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
						vconsole_deactivate(&tty_console[tty_console_cur]);
						tty_console_cur = token->params[0]-11;
						tty_go_left( cur_tty, 7 ); /* erase `TTY-X$' */
						printf("TTY-%d$ ",tty_console_cur+1);
						vconsole_activate(&tty_console[tty_console_cur]);
					}
				#endif
				break;
			}
		break;

		case VT_ACTION_EXECUTE:
			switch (token->ch) {
				case '\n': /* ENTER key. Return string */
					if (cur_tty->out_busy) break;
					/* add string to output buffer */
					memcpy((void*) cur_tty->out_buff,(const void*)
						cur_tty->rx_buff, cur_tty->rx_cnt);
					cur_tty->out_buff[cur_tty->rx_cnt] = '\0';
					cur_tty->rx_cnt = 0;
					cur_tty->rx_cur = 0;
					cur_tty->out_busy = true;
					vtbuild(cur_tty->vtb, token);
				break;

				case 21: /* ^U clean line */ {
				#if 0
					uint32_t i;
					for (i=0; i<cur_tty->rx_cur; ++i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i+cur_tty->rx_cur];
					}
					for (i=cur_tty->rx_cur; i>0; --i) {
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
					}
					cur_tty->rx_cnt -= cur_tty->rx_cur;
					for (i=0; i<cur_tty->rx_cnt; ++i) {
						cur_tty->file_op->fwrite(&cur_tty->rx_buff[i],sizeof(char),1,NULL);
					}
					for (i=0; i<cur_tty->rx_cur; ++i) {
						cur_tty->file_op->fwrite(" ",sizeof(char),1,NULL);
					}
					for (i=cur_tty->rx_cur+cur_tty->rx_cnt; i>0; --i) {
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
					}
					cur_tty->rx_cur = 0;
				#else
					copy_forward(
						&cur_tty->rx_buff[cur_tty->rx_cur],
						&cur_tty->rx_buff[0],
						cur_tty->rx_cnt - cur_tty->rx_cur );
					tty_rewrite_line( cur_tty,
						&cur_tty->rx_buff[0],
						cur_tty->rx_cnt - cur_tty->rx_cur,
						cur_tty->rx_cnt, cur_tty->rx_cur, 0);
					cur_tty->rx_cnt -= cur_tty->rx_cur;
					cur_tty->rx_cur = 0;
				#endif
				} break;

				case 23: /* ^W remove last word */ {
				#if 0
					uint32_t i,tps; /* tps - to position space */
					for (tps=cur_tty->rx_cur; tps>0 && tty_isalpha(cur_tty->rx_buff[tps]); --tps);
					for (; tps>0 && tty_isspace(cur_tty->rx_buff[tps]); --tps);
					if (tps>0) {++tps;}

					for (i=tps; i<cur_tty->rx_cur; ++i) {
						cur_tty->rx_buff[i] = cur_tty->rx_buff[i+cur_tty->rx_cur-tps];
					}
					for (i=cur_tty->rx_cur; i>tps; --i) {
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
					}
					cur_tty->rx_cnt -= cur_tty->rx_cur-tps;
					for (i=tps; i<cur_tty->rx_cnt; ++i) {
						cur_tty->file_op->fwrite(&cur_tty->rx_buff[i],sizeof(char),1,NULL);
					}
					for (i=tps; i<cur_tty->rx_cur; ++i) {
						cur_tty->file_op->fwrite(" ",sizeof(char),1,NULL);
					}
					for (i=cur_tty->rx_cnt+cur_tty->rx_cur-tps; i>tps; --i) {
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
					}
					cur_tty->rx_cur = tps;
				#else
					/* find end of prev word */
					uint32_t tps; /* tps - to position space */
					tps = cur_tty->rx_cur>0 ? cur_tty->rx_cur-1 : 0;
					for (; tps>0 && tty_isalpha(cur_tty->rx_buff[tps]); --tps);
					for (; tps>0 && tty_isspace(cur_tty->rx_buff[tps]); --tps);
					if (tps>0) {++tps;}

					copy_forward(
						&cur_tty->rx_buff[cur_tty->rx_cur],
						&cur_tty->rx_buff[tps],
						cur_tty->rx_cnt - cur_tty->rx_cur + tps );

					tty_rewrite_line( cur_tty,
						&cur_tty->rx_buff[tps],
						cur_tty->rx_cnt - cur_tty->rx_cur ,
						cur_tty->rx_cnt - tps,
						cur_tty->rx_cur - tps, 0 );

					cur_tty->rx_cnt -= cur_tty->rx_cur - tps;
					cur_tty->rx_cur = tps;

				#endif
				}
				break;

				case 8: /* ^H equalent to backspace */
				case 127: /* backspace */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						vtbuild(cur_tty->vtb, TOKEN_LEFT);
						uint32_t i;
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt-1; ++i) {
							cur_tty->rx_buff[i] = cur_tty->rx_buff[i+1];
							cur_tty->file_op->fwrite(&cur_tty->rx_buff[i],sizeof(char),1,NULL);
						}
						cur_tty->file_op->fwrite(" ",sizeof(char),1,NULL);
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
							vtbuild(cur_tty->vtb, TOKEN_LEFT);
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
	cur_tty->file_op->fwrite(&ch,sizeof(char),1,NULL);
}

static int tty_init_flag = 0;

int tty_init(void) {
	if (NULL == vtparse_init(cur_tty->vtp, tty_vtparse_callback)) {
		LOG_ERROR("Error while initialization vtparse.\n");
	}
	if (NULL == vtbuild_init(cur_tty->vtb, tty_vtbuild_callback)) {
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
	vtparse(cur_tty->vtp, ch);
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


