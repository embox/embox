/* Code and data for the IBM console driver.
 *
 * The 6845 video controller used by the IBM PC shares its video memory with
 * the CPU somewhere in the 0xB0000 memory bank.  To the 6845 this memory
 * consists of 16-bit words.  Each word has a character code in the low byte
 * and a so-called attribute byte in the high byte.  The CPU directly modifies
 * video memory to display characters, and sets two registers on the 6845 that
 * specify the video origin and the cursor position.  The video origin is the
 * place in video memory where the first character (upper left corner) can
 * be found.  Moving the origin is a fast way to scroll the screen.  Some
 * video adapters wrap around the top of video memory, so the origin can
 * move without bounds.  For other adapters screen memory must sometimes be
 * moved to reset the origin.  All computations on video memory use character
 * (word) addresses for simplicity and assume there is no wrapping.  The
 * assembly support functions translate the word addresses to byte addresses
 * and the scrolling function worries about wrapping.
 */
/**
 * @file
 *
 *  @brief Embox Posix console device interface.
 *  Changed by Gerald Hoch (20.08.2011)
 *
 */

/**
 * CONFIG parameters used
 *
 * - CONFIG_NR_CONS
 *
 */

/**
 * include system headers
 */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <embox/unit.h>
#include <embox/device.h>
#include <embox/service/callback.h>
#include <asm/io.h>
#include <kernel/panic.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <hal/reg.h>
#include <fs/node.h>
#include <fs/file.h>
#include <fs/vfs.h>
/**
 * include TTY specific headers
 */
#include <drivers/tty_posix/const.h>
#include <drivers/tty_posix/termios.h>
#include <drivers/tty_posix/tty.h>
#include <drivers/tty_posix/console.h>


/*
 * device functions
 */
static void 	*open_factory(const char *fname, const char *_mode);
static int 		close_factory(void *file);
static size_t 	read_factory(void *buf, size_t size, size_t count, void *file);
static size_t 	write_factory(const void *buff, size_t size, size_t count, void *file);
static int 		ioctl_factory(void *file, int request, va_list args);
static void 	*open(const char *fname, const char *_mode);
static int 		close(void *file);
static size_t 	read(void *buf, size_t size, size_t count, void *file);
static size_t 	write(const void *buff, size_t size, size_t count, void *file);
static int 		ioctl(void *file, int request, va_list args);

static console_t* 	getConsole	(void* fname);

static int 			getLine		(const char* fname);

static struct tty * getTty		(void* file);
static void 		mem_vid_copy(u16_t *src, unsigned dst, unsigned count);
static void 		vid_vid_copy(unsigned src, unsigned dst, unsigned count);
PUBLIC void 		scr_init	(struct tty *tp,bool clear_screen);
PUBLIC void 		kb_init		(struct tty *tp);
PUBLIC void 		kb_init_once(void);

static void 		vid_mem_copy(unsigned src, u16_t * dst, unsigned count);
static void			set_fg(console_t* cons,int fg);
static void			set_bg(console_t* cons,int bg);

volatile static bool	diagMode 	= false;		// dont optimize
static void putk(int c);
static bool ega 	= false;
static phys_bytes vid_base;	/* video base */


#if USE_BEEP
static bool			beeping;
#endif
/*
 * Types
 */
static file_operations_t  file_op_factory = {
	.fread 	= read_factory,
	.fopen 	= open_factory,
	.fclose = close_factory,
	.fwrite = write_factory,
	.ioctl  = ioctl_factory
};
static file_operations_t  file_op = {
	.fread 	= read,
	.fopen 	= open,
	.fclose = close,
	.fwrite = write,
	.ioctl  = ioctl
};
static file_system_driver_t devfs_drv = {
		.name 		= "console",
		.file_op 	= &file_op,
		.fsop		= NULL
};
/**
 * console is a device
 */
EMBOX_DEVICE("console", &file_op_factory);

/**
 * diag test for screen
 */
int tty_posix_console_diag_init(void)
{
//	struct tty * tp;
	u16_t *pSave_videoram;
	console_t *cons;
	node_t * node;
	FILE *f,*f0,*fk;

	diagMode = true;

	// create all console devices
	f = fopen("/dev/console", "w");
	if (NULL == f) {
		LOG_ERROR("/dev/console has not registered!\n");
		return -1;
	}
	f0 = fopen("/dev/console/0", "w");
	if (NULL == f0) {
		LOG_ERROR("/dev/console/0 has not registered!\n");
		return -1;
	}
	node = (node_t*) f0;
	cons = (console_t *)node->attr;

	fk = fopen("/dev/kbd", "r");
	if (NULL == fk) {
		LOG_ERROR("/dev/kbd has not registered!\n");
		return -1;
	}

	// save the actual videoram to local store
	pSave_videoram = malloc(scr_size);
	vid_mem_copy(0,pSave_videoram,2000);

	// clear screen
	mem_vid_copy(BLANK_MEM, cons->c_start, scr_size);

	set_fg(cons,14);
	set_bg(cons,1);
	fwrite("console test...\r\n",17,1,f0);

	sleep(1);

	fclose(f0);
	fclose(f);
	fclose(fk);

	// restore orig videoram
	mem_vid_copy(pSave_videoram,0,scr_size);

	free(pSave_videoram);

	diagMode = false;

	return 1;
}


// ##################################################################################
// console factory
// ##################################################################################
/*
 * device file_operation OPEN
 * @return console_t*
 *
 * fname could be 'console'
 * nod->attr - will be used for console_t* !!!
 */
static void *open_factory(const char *fname, const char *_mode) {
//	struct tty *tp;
//	int line;
	node_t * node_factory;
	node_t * node_line;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	console_t *cons;

	if (strcmp(strupr((char *)mode),"W") != 0)
		return NULL;

	node_factory = vfs_find_node("/dev/console",NULL);
	if (node_factory == NULL)
		return (void *)NULL;

	//factory creates all console CONFIG_NR_CONS
	for (int i=0;i<CONFIG_NR_CONS;i++) {
		sprintf(node_name,"%d",i);

		node_line 	= vfs_find_child(node_name,node_factory);
		if (node_line == NULL) {
			node_line 				= vfs_add_path(node_name,node_factory);
			node_line->file_info 	= (void*) &file_op;
			cons 					= &cons_table[i];
			node_line->attr 		= cons;
			cons->cons_line			= i;
			node_line->fs_type 		= &devfs_drv;
		}
	}
	return (void *)node_factory;
}
static int close_factory(void *file) {
	return 0;
}
static size_t read_factory(void *buf, size_t size, size_t count, void *file) {
	return 0;
}
static size_t write_factory(const void *buff, size_t size, size_t count, void *file) {
	return 0;
}
static int ioctl_factory(void *file, int request, va_list args) {
	return 0;
}
// ##################################################################################
// - console instances
/*
 * device file_operation OPEN
 * @return console_t*
 *
 * fname could be '0'...'4'
 * nod->attr - will be used for console_t* !!!
 */
static void *open(const char *fname, const char *_mode) {
	struct tty *tp;
	int line;
	node_t * node_factory;
	node_t * node_line;
	char node_name[CONFIG_MAX_LENGTH_FILE_NAME];
	console_t *cons;
	char *p_path;

	if (strcmp(strupr((char *)mode),"W") != 0)
		return NULL;

	node_factory = vfs_find_node("/dev/console",NULL);
	if (node_factory == NULL)
		return NULL;

	p_path = get_next_node_name(fname, node_name, sizeof(node_name));
	p_path = get_next_node_name(p_path, node_name, sizeof(node_name));

	line = getLine(p_path);
	if (line < 0)
		return NULL;

	node_line 	= vfs_find_child(p_path,node_factory);
	if (node_line == NULL)
		return NULL;

	// link both together
	cons = node_line->attr;
	tp = getTTY_Cons(line);
	cons->c_tty = tp;
	tp->tty_priv = cons;

	// ###############################################
	// init screen instance
	// ------------------------------------------------
	if (diagMode)
		scr_init(tp,false);
	else
		scr_init(tp,true);

	return node_line;
}
/*
 * define line from devicename
 */
static int getLine(const char* fname) {
	int line;

	if(strlen(fname) <= 2) {
		// 0 -  system console
		sscanf((char*)fname,"%d",&line);
	} else {
		line = -1;
	}

	if (line >= nr_cons)
	  return -1;
	return line;
}
/**
 * get console struct based on line(fname)
 */
static console_t* getConsole(void* file) {
	node_t * node_line = file;

	return (console_t*) node_line->attr;
}
/**
 * get tty structure
 */
static struct tty * getTty(void* file) {
	console_t * cons;
	struct tty * tp;

	cons= getConsole(file);
	tp 	= cons->c_tty;

	return tp;
}
static int close(void *file) {
	//console_t* cons = getConsole(file);
	//struct tty *tp = getTty(file);

	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {

	return 0;
}

static size_t write(const void *buff, size_t size, size_t count, void *file) {
	for (int j = 0;j<count;j++)
	{
		for (int i = 0;i<size;i++)
		{
			putk(*((char*)buff+i));
		}
	}
	putk(0);
	return 0;
}

static int ioctl(void *file, int request, va_list args) {
//	console_t* cons = getConsole(file);
	struct tty *tp = getTty(file);
	if (tp == NULL)	return -1;

	cons_ioctl(tp);
	return 0;
}
void set_fg(console_t* cons,int fg)
/*
 *
0x0 Schwarz 	0x8 Dunkelgrau
0x1 Blau 		0x9 Hellblau
0x2 Gr�n 		0xA Hellgr�n
0x3 Cyan 		0xB	Hellcyan
0x4 Rot 		0xC Hellrot
0x5 Magenta 	0xD Hellmagenta
0x6 Braun 		0xE	Gelb
0x7 Hellgrau	0xF Wei�
*/
{
	cons->c_attr = (cons->c_attr  & 0xF8FF)| (fg << 8);
}
void set_bg(console_t* cons,int bg)
{
	cons->c_attr = (cons->c_attr  & 0x8FFF)| (bg << 12);
}
/*===========================================================================*
 *				scr_init				     *
 *===========================================================================*/
PUBLIC void scr_init(tty_t * tp,bool clear_screen)
{
/* Initialize the screen driver. */
  console_t *cons;
  u16_t bios_columns, bios_crtbase, bios_fontlines;
  u8_t bios_rows;
  int line;
  unsigned page_size;

  /* Associate console and TTY. */
  cons = (console_t *)tp->tty_priv;
  line = cons->cons_line;

  /**
   *  Initialize the keyboard driver.
   * new console will get the keyboard :)
   *
   * */

  /* Fill in TTY function hooks. */
  tp->tty_devwrite 	= cons_write;
  tp->tty_echo 		= cons_echo;
  tp->tty_ioctl 	= cons_ioctl;

  if (line == 0) {
	  /* Get the BIOS parameters that describe the VDU. */
	  *((u16_t*)&bios_columns) 	= *((u16_t*)0x44AL);
	  *((u16_t*)&bios_crtbase) 	= *((u16_t*)0x463L);
	  *((u8_t*)&bios_rows) 		= *((u8_t*)0x484L);
	  *((u16_t*)&bios_fontlines)= *((u16_t*)0x485L);

	  vid_port 		= bios_crtbase;
	  scr_width 	= bios_columns;
	  font_lines 	= bios_fontlines;
	  scr_lines 	= ega ? bios_rows+1 : 25;

	  if (color) {
		vid_base = COLOR_BASE;
		vid_size = COLOR_SIZE;
	  } else {
		vid_base = MONO_BASE;
		vid_size = MONO_SIZE;
	  }
	  if (ega) vid_size = EGA_SIZE;
	  wrap = !ega;

	  vid_size >>= 1;		/* word count */
	  vid_mask = vid_size - 1;

	  /* Size of the screen (number of displayed characters.) */
	  scr_size = scr_lines * scr_width;

	  /* There can be as many consoles as video memory allows. */
	  nr_cons = vid_size / scr_size;
	  if (nr_cons > CONFIG_NR_CONS) nr_cons = CONFIG_NR_CONS;
	  if (nr_cons > 1) wrap = 0;
	  page_size = vid_size / nr_cons;
	  blank_color = BLANK_COLOR;
  }
  cons->c_start = line * page_size;
  cons->c_limit = cons->c_start + page_size;
  cons->c_cur = cons->c_org = cons->c_start;
  cons->c_attr = cons->c_blank = BLANK_COLOR;

  /* Clear the screen. */
  if (clear_screen)
	  mem_vid_copy(BLANK_MEM, cons->c_start, scr_size);

  /* select 1 console */
  select_console(line);

  /* set termios parm */
  cons_ioctl(tp);
}

/*===========================================================================*
 *				set_6845				     *
 *===========================================================================*/
/* Set a register pair inside the 6845.
 * Registers 12-13 tell the 6845 where in video ram to start
 * Registers 14-15 tell the 6845 where to put the cursor
 */
PRIVATE void set_6845(reg, val)
int reg;			/* which register pair to set */
int val;			/* 16-bit value to set it to */
{
	volatile crtc_regs_t * crtc_reg = (volatile crtc_regs_t *) vid_port;

	out16((val & 0xff00) | reg, crtc_reg);
	out16((val <<     8) | (reg+1), crtc_reg);

}

/*===========================================================================*
 *				cons_write				     *
 *===========================================================================*/
PRIVATE void cons_write(tp)
register struct tty *tp;	/* tells which terminal is to be used */
{
/* Copy as much data as possible to the output queue, then start I/O.  On
 * memory-mapped terminals, such as the IBM console, the I/O will also be
 * finished, and the counts updated.  Keep repeating until all I/O done.
 */

  int count;
  register char *tbuf;
  char buf[64];
  //phys_bytes user_phys;
  console_t *cons = tp->tty_priv;

  /* Check quickly for nothing to do, so this can be called often without
   * unmodular tests elsewhere.
   */
  if ((count = tp->tty_outleft) == 0 || tp->tty_inhibited) return;

  /* Copy the user bytes to buf[] for decent addressing. Loop over the
   * copies, since the user buffer may be much larger than buf[].
   */
  do {
	if (count > sizeof(buf)) count = sizeof(buf);
	/*user_phys = proc_vir2phys(proc_addr(tp->tty_outproc), tp->tty_out_vir);
	phys_copy(user_phys, vir2phys(buf), (phys_bytes) count);*/

	memcpy((u8_t*)tp->tty_out_vir,buf,count);
	tbuf = buf;

	/* Update terminal data structure. */
	tp->tty_out_vir += count;
	tp->tty_outcum += count;
	tp->tty_outleft -= count;

	/* Output each byte of the copy to the screen.  Avoid calling
	 * out_char() for the "easy" characters, put them into the buffer
	 * directly.
	 */
	do {
		if ((unsigned) *tbuf < ' ' || cons->c_esc_state > 0
			|| cons->c_column >= scr_width
			|| cons->c_rwords >= buflen(cons->c_ramqueue))
		{
			out_char(cons, *tbuf++);
		} else {
			cons->c_ramqueue[cons->c_rwords++] = cons->c_attr | (*tbuf++ & 0xff);
			cons->c_column++;
		}
	} while (--count != 0);
  } while ((count = tp->tty_outleft) != 0 && !tp->tty_inhibited);

  flush(cons);			/* transfer anything buffered to the screen */

  /* Reply to the writer if all output is finished. */
    if (tp->tty_outleft == 0) {
  	tty_reply(tp->tty_outrepcode, tp->tty_outcaller, tp->tty_outproc,tp->tty_outcum);
  	tp->tty_outcum = 0;
    }
}


/*===========================================================================*
 *				cons_echo				     *
 *===========================================================================*/
PRIVATE void cons_echo(tp, c)
register tty_t *tp;		/* pointer to tty struct */
int c;				/* character to be echoed */
{
/* Echo keyboard input (print & flush). */
  console_t *cons = tp->tty_priv;

  out_char(cons, c);
  flush(cons);
}


/*===========================================================================*
 *				out_char				     *
 *===========================================================================*/
PRIVATE void out_char(cons, c)
register console_t *cons;	/* pointer to console struct */
int c;				/* character to be output */
{
/* Output a character on the console.  Check for escape sequences first. */
  if (cons->c_esc_state > 0) {
	parse_escape(cons, c);
	return;
  }

  switch(c) {
	case 000:		/* null is typically used for padding */
		return;		/* better not do anything */

	case 007:		/* ring the bell */
		flush(cons);	/* print any chars queued for output */
#if USE_BEEP
		beep();
#endif
		return;

	case '\b':		/* backspace */
		if (--cons->c_column < 0) {
			if (--cons->c_row >= 0) cons->c_column += scr_width;
		}
		flush(cons);
		return;

	case '\n':		/* line feed */
		if ((cons->c_tty->tty_termios.c_oflag & (OPOST|ONLCR))
						== (OPOST|ONLCR)) {
			cons->c_column = 0;
		}
		/*FALL THROUGH*/
	case 013:		/* CTRL-K */
	case 014:		/* CTRL-L */
		if (cons->c_row == scr_lines-1) {
			scroll_screen(cons, SCROLL_UP);
		} else {
			cons->c_row++;
		}
		flush(cons);
		return;

	case '\r':		/* carriage return */
		cons->c_column = 0;
		flush(cons);
		return;

	case '\t':		/* tab */
		cons->c_column = (cons->c_column + TAB_SIZE) & ~TAB_MASK;
		if (cons->c_column > scr_width) {
			cons->c_column -= scr_width;
			if (cons->c_row == scr_lines-1) {
				scroll_screen(cons, SCROLL_UP);
			} else {
				cons->c_row++;
			}
		}
		flush(cons);
		return;

	case 033:		/* ESC - start of an escape sequence */
		flush(cons);	/* print any chars queued for output */
		cons->c_esc_state = 1;	/* mark ESC as seen */
		return;

	default:		/* printable chars are stored in ramqueue */
		if (cons->c_column >= scr_width) {
			if (!LINEWRAP) return;
			if (cons->c_row == scr_lines-1) {
				scroll_screen(cons, SCROLL_UP);
			} else {
				cons->c_row++;
			}
			cons->c_column = 0;
			flush(cons);
		}
		if (cons->c_rwords == buflen(cons->c_ramqueue)) flush(cons);
		cons->c_ramqueue[cons->c_rwords++] = cons->c_attr | (c & 0xff);
		cons->c_column++;			/* next column */
		return;
  }
}


/*===========================================================================*
 *				scroll_screen				     *
 *===========================================================================*/
PRIVATE void scroll_screen(cons, dir)
register console_t *cons;	/* pointer to console struct */
int dir;			/* SCROLL_UP or SCROLL_DOWN */
{
  unsigned new_line, new_org, chars;

  flush(cons);
  chars = scr_size - scr_width;		/* one screen minus one line */

  /* Scrolling the screen is a real nuisance due to the various incompatible
   * video cards.  This driver supports software scrolling (Hercules?),
   * hardware scrolling (mono and CGA cards) and hardware scrolling without
   * wrapping (EGA cards).  In the latter case we must make sure that
   *		c_start <= c_org && c_org + scr_size <= c_limit
   * holds, because EGA doesn't wrap around the end of video memory.
   */
  if (dir == SCROLL_UP) {
	/* Scroll one line up in 3 ways: soft, avoid wrap, use origin. */
	if (softscroll) {
		vid_vid_copy(cons->c_start + scr_width, cons->c_start, chars);
	} else
	if (!wrap && cons->c_org + scr_size + scr_width >= cons->c_limit) {
		vid_vid_copy(cons->c_org + scr_width, cons->c_start, chars);
		cons->c_org = cons->c_start;
	} else {
		cons->c_org = (cons->c_org + scr_width) & vid_mask;
	}
	new_line = (cons->c_org + chars) & vid_mask;
  } else {
	/* Scroll one line down in 3 ways: soft, avoid wrap, use origin. */
	if (softscroll) {
		vid_vid_copy(cons->c_start, cons->c_start + scr_width, chars);
	} else
	if (!wrap && cons->c_org < cons->c_start + scr_width) {
		new_org = cons->c_limit - scr_size;
		vid_vid_copy(cons->c_org, new_org + scr_width, chars);
		cons->c_org = new_org;
	} else {
		cons->c_org = (cons->c_org - scr_width) & vid_mask;
	}
	new_line = cons->c_org;
  }
  /* Blank the new line at top or bottom. */
  blank_color = cons->c_blank;
  mem_vid_copy(BLANK_MEM, new_line, scr_width);

  /* Set the new video origin. */
  if (cons == curcons) set_6845(VID_ORG, cons->c_org);
  flush(cons);
}


/*===========================================================================*
 *				flush					     *
 *===========================================================================*/
PRIVATE void flush(cons)
register console_t *cons;	/* pointer to console struct */
{
/* Send characters buffered in 'ramqueue' to screen memory, check the new
 * cursor position, compute the new hardware cursor position and set it.
 */
  unsigned cur;
  tty_t *tp = cons->c_tty;

  /* Have the characters in 'ramqueue' transferred to the screen. */
  if (cons->c_rwords > 0) {
	mem_vid_copy(cons->c_ramqueue, cons->c_cur, cons->c_rwords);
	cons->c_rwords = 0;

	/* TTY likes to know the current column and if echoing messed up. */
	tp->tty_position = cons->c_column;
	tp->tty_reprint = TRUE;
  }

  /* Check and update the cursor position. */
  if (cons->c_column < 0) cons->c_column = 0;
  if (cons->c_column > scr_width) cons->c_column = scr_width;
  if (cons->c_row < 0) cons->c_row = 0;
  if (cons->c_row >= scr_lines) cons->c_row = scr_lines - 1;
  cur = cons->c_org + cons->c_row * scr_width + cons->c_column;
  if (cur != cons->c_cur) {
	if (cons == curcons) set_6845(CURSOR, cur);
	cons->c_cur = cur;
  }
}


/*===========================================================================*
 *				parse_escape				     *
 *===========================================================================*/
PRIVATE void parse_escape(cons, c)
register console_t *cons;	/* pointer to console struct */
char c;				/* next character in escape sequence */
{
/* The following ANSI escape sequences are currently supported.
 * If n and/or m are omitted, they default to 1.
 *   ESC [nA moves up n lines
 *   ESC [nB moves down n lines
 *   ESC [nC moves right n spaces
 *   ESC [nD moves left n spaces
 *   ESC [m;nH" moves cursor to (m,n)
 *   ESC [J clears screen from cursor
 *   ESC [K clears line from cursor
 *   ESC [nL inserts n lines ar cursor
 *   ESC [nM deletes n lines at cursor
 *   ESC [nP deletes n chars at cursor
 *   ESC [n@ inserts n chars at cursor
 *   ESC [nm enables rendition n (0=normal, 4=bold, 5=blinking, 7=reverse)
 *   ESC M scrolls the screen backwards if the cursor is on the top line
 */

  switch (cons->c_esc_state) {
    case 1:			/* ESC seen */
	cons->c_esc_intro = '\0';
	cons->c_esc_parmp = bufend(cons->c_esc_parmv);
	do {
		*--cons->c_esc_parmp = 0;
	} while (cons->c_esc_parmp > cons->c_esc_parmv);
	switch (c) {
	    case '[':	/* Control Sequence Introducer */
		cons->c_esc_intro = c;
		cons->c_esc_state = 2;
		break;
	    case 'M':	/* Reverse Index */
		do_escape(cons, c);
		break;
	    default:
		cons->c_esc_state = 0;
	}
	break;

    case 2:			/* ESC [ seen */
	if (c >= '0' && c <= '9') {
		if (cons->c_esc_parmp < bufend(cons->c_esc_parmv))
			*cons->c_esc_parmp = *cons->c_esc_parmp * 10 + (c-'0');
	} else
	if (c == ';') {
		if (cons->c_esc_parmp < bufend(cons->c_esc_parmv))
			cons->c_esc_parmp++;
	} else {
		do_escape(cons, c);
	}
	break;
  }
}


/*===========================================================================*
 *				do_escape				     *
 *===========================================================================*/
PRIVATE void do_escape(cons, c)
register console_t *cons;	/* pointer to console struct */
char c;				/* next character in escape sequence */
{
  int value, n;
  unsigned src, dst, count;
  int *parmp;

  /* Some of these things hack on screen RAM, so it had better be up to date */
  flush(cons);

  if (cons->c_esc_intro == '\0') {
	/* Handle a sequence beginning with just ESC */
	switch (c) {
	    case 'M':		/* Reverse Index */
		if (cons->c_row == 0) {
			scroll_screen(cons, SCROLL_DOWN);
		} else {
			cons->c_row--;
		}
		flush(cons);
		break;

	    default: break;
	}
  } else
  if (cons->c_esc_intro == '[') {
	/* Handle a sequence beginning with ESC [ and parameters */
	value = cons->c_esc_parmv[0];
	switch (c) {
	    case 'A':		/* ESC [nA moves up n lines */
		n = (value == 0 ? 1 : value);
		cons->c_row -= n;
		flush(cons);
		break;

	    case 'B':		/* ESC [nB moves down n lines */
		n = (value == 0 ? 1 : value);
		cons->c_row += n;
		flush(cons);
		break;

	    case 'C':		/* ESC [nC moves right n spaces */
		n = (value == 0 ? 1 : value);
		cons->c_column += n;
		flush(cons);
		break;

	    case 'D':		/* ESC [nD moves left n spaces */
		n = (value == 0 ? 1 : value);
		cons->c_column -= n;
		flush(cons);
		break;

	    case 'H':		/* ESC [m;nH" moves cursor to (m,n) */
		cons->c_row = cons->c_esc_parmv[0] - 1;
		cons->c_column = cons->c_esc_parmv[1] - 1;
		flush(cons);
		break;

	    case 'J':		/* ESC [sJ clears in display */
		switch (value) {
		    case 0:	/* Clear from cursor to end of screen */
			count = scr_size - (cons->c_cur - cons->c_org);
			dst = cons->c_cur;
			break;
		    case 1:	/* Clear from start of screen to cursor */
			count = cons->c_cur - cons->c_org;
			dst = cons->c_org;
			break;
		    case 2:	/* Clear entire screen */
			count = scr_size;
			dst = cons->c_org;
			break;
		    default:	/* Do nothing */
			count = 0;
			dst = cons->c_org;
		}
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, dst, count);
		break;

	    case 'K':		/* ESC [sK clears line from cursor */
		switch (value) {
		    case 0:	/* Clear from cursor to end of line */
			count = scr_width - cons->c_column;
			dst = cons->c_cur;
			break;
		    case 1:	/* Clear from beginning of line to cursor */
			count = cons->c_column;
			dst = cons->c_cur - cons->c_column;
			break;
		    case 2:	/* Clear entire line */
			count = scr_width;
			dst = cons->c_cur - cons->c_column;
			break;
		    default:	/* Do nothing */
			count = 0;
			dst = cons->c_cur;
		}
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, dst, count);
		break;

	    case 'L':		/* ESC [nL inserts n lines at cursor */
		n = value;
		if (n < 1) n = 1;
		if (n > (scr_lines - cons->c_row))
			n = scr_lines - cons->c_row;

		src = cons->c_org + cons->c_row * scr_width;
		dst = src + n * scr_width;
		count = (scr_lines - cons->c_row - n) * scr_width;
		vid_vid_copy(src, dst, count);
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, src, n * scr_width);
		break;

	    case 'M':		/* ESC [nM deletes n lines at cursor */
		n = value;
		if (n < 1) n = 1;
		if (n > (scr_lines - cons->c_row))
			n = scr_lines - cons->c_row;

		dst = cons->c_org + cons->c_row * scr_width;
		src = dst + n * scr_width;
		count = (scr_lines - cons->c_row - n) * scr_width;
		vid_vid_copy(src, dst, count);
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, dst + count, n * scr_width);
		break;

	    case '@':		/* ESC [n@ inserts n chars at cursor */
		n = value;
		if (n < 1) n = 1;
		if (n > (scr_width - cons->c_column))
			n = scr_width - cons->c_column;

		src = cons->c_cur;
		dst = src + n;
		count = scr_width - cons->c_column - n;
		vid_vid_copy(src, dst, count);
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, src, n);
		break;

	    case 'P':		/* ESC [nP deletes n chars at cursor */
		n = value;
		if (n < 1) n = 1;
		if (n > (scr_width - cons->c_column))
			n = scr_width - cons->c_column;

		dst = cons->c_cur;
		src = dst + n;
		count = scr_width - cons->c_column - n;
		vid_vid_copy(src, dst, count);
		blank_color = cons->c_blank;
		mem_vid_copy(BLANK_MEM, dst + count, n);
		break;

	    case 'm':		/* ESC [nm enables rendition n */
		for (parmp = cons->c_esc_parmv; parmp <= cons->c_esc_parmp
				&& parmp < bufend(cons->c_esc_parmv); parmp++) {
			if (cons->c_reverse) {
				/* Unswap fg and bg colors */
				cons->c_attr =	((cons->c_attr & 0x7000) >> 4) |
						((cons->c_attr & 0x0700) << 4) |
						((cons->c_attr & 0x8800));
			}
			switch (n = *parmp) {
			    case 0:	/* NORMAL */
				cons->c_attr = cons->c_blank = BLANK_COLOR;
				cons->c_reverse = FALSE;
				break;

			    case 1:	/* BOLD  */
				/* Set intensity bit */
				cons->c_attr |= 0x0800;
				break;

			    case 4:	/* UNDERLINE */
				if (color) {
					/* Change white to cyan, i.e. lose red
					 */
					cons->c_attr = (cons->c_attr & 0xBBFF);
				} else {
					/* Set underline attribute */
					cons->c_attr = (cons->c_attr & 0x99FF);
				}
				break;

			    case 5:	/* BLINKING */
				/* Set the blink bit */
				cons->c_attr |= 0x8000;
				break;

			    case 7:	/* REVERSE */
				cons->c_reverse = TRUE;
				break;

			    default:	/* COLOR */
				if (n == 39) n = 37;	/* set default color */
				if (n == 49) n = 40;

				if (!color) {
					/* Don't mess up a monochrome screen */
				} else
				if (30 <= n && n <= 37) {
					/* Foreground color */
					cons->c_attr = (cons->c_attr  & 0xF8FF)| (ansi_colors[(n - 30)] << 8);
					cons->c_blank= (cons->c_blank & 0xF8FF)| (ansi_colors[(n - 30)] << 8);
				} else
				if (40 <= n && n <= 47) {
					/* Background color */
					cons->c_attr = (cons->c_attr  & 0x8FFF)| (ansi_colors[(n - 40)] << 12);
					cons->c_blank= (cons->c_blank & 0x8FFF)| (ansi_colors[(n - 40)] << 12);
				}
			}
			if (cons->c_reverse) {
				/* Swap fg and bg colors */
				cons->c_attr =	((cons->c_attr & 0x7000) >> 4) |
						((cons->c_attr & 0x0700) << 4) |
						((cons->c_attr & 0x8800));
			}
		}
		break;
	}
  }
  cons->c_esc_state = 0;
}


/*===========================================================================*
 *				beep					     *
 *===========================================================================*/
#if USE_BEEP
PRIVATE void beep()
{
/* Making a beeping sound on the speaker (output for CRTL-G).
 * This routine works by turning on the bits 0 and 1 in port B of the 8255
 * chip that drive the speaker.
 */
  static timer_t tmr_stop_beep;

  if (!beeping) {
	outb(TIMER_MODE, 0xB6);		/* timer channel 2, square wave */
	outb(TIMER2, (BEEP_FREQ >> 0) & BYTE);	/* low freq byte */
	outb(TIMER2, (BEEP_FREQ >> 8) & BYTE);	/* high freq byte */
	lock();			/* guard PORT_B from keyboard intr handler */
	outb(PORT_B, inb(PORT_B) | 3);		/* turn on beep bits */
	unlock();
	beeping = TRUE;
  }
  tmr_settimer(&tmr_stop_beep, TTY, get_uptime()+B_TIME, stop_beep);
}
#endif

/*===========================================================================*
 *				stop_beep				     *
 *===========================================================================*/
#if USE_BEEP
PRIVATE void stop_beep(tmrp)
sys_timer_t *tmrp;
{
/* Turn off the beeper by turning off bits 0 and 1 in PORT_B. */

  lock();			/* guard PORT_B from keyboard intr handler */
  outb(PORT_B, inb(PORT_B) & ~3);
  beeping = FALSE;
  unlock();
}
#endif


/*===========================================================================*
 *				putk	character to print				     *
 *===========================================================================*/
static void putk(int c)
{
/* This procedure is used by the version of printf() that is linked with
 * the kernel itself.  The one in the library sends a message to FS, which is
 * not what is needed for printing within the kernel.  This version just queues
 * the character and starts the output.
 */

  if (c != 0) {
	if (c == '\n') putk('\r');
	out_char(&cons_table[0], (int) c);
  } else {
	flush(&cons_table[0]);
  }
}


/*===========================================================================*
 *				toggle_scroll				     *
 *===========================================================================*/
PUBLIC void toggle_scroll(void)
{
/* Toggle between hardware and software scroll. */

  cons_org0();
  softscroll = !softscroll;
  printf("%sware scrolling enabled.\n", softscroll ? "Soft" : "Hard");
}


/*===========================================================================*
 *				cons_stop				     *
 *===========================================================================*/
PUBLIC void cons_stop(void)
{
/* Prepare for halt or reboot. */
  cons_org0();
  softscroll = 1;
  select_console(0);
  cons_table[0].c_attr = cons_table[0].c_blank = BLANK_COLOR;
}


/*===========================================================================*
 *				cons_org0				     *
 *===========================================================================*/
PRIVATE void cons_org0()
{
/* Scroll video memory back to put the origin at 0. */
  int cons_line;
  console_t *cons;
  unsigned n;

  for (cons_line = 0; cons_line < nr_cons; cons_line++) {
	cons = &cons_table[cons_line];
	while (cons->c_org > cons->c_start) {
		n = vid_size - scr_size;	/* amount of unused memory */
		if (n > cons->c_org - cons->c_start)
			n = cons->c_org - cons->c_start;
		vid_vid_copy(cons->c_org, cons->c_org - n, scr_size);
		cons->c_org -= n;
	}
	flush(cons);
  }
  select_console(current);
}


/*===========================================================================*
 *				select_console				     *
 *===========================================================================*/
PUBLIC void select_console(int cons_line)
{
/* Set the current console to console number 'cons_line'. */

  if (cons_line < 0 || cons_line >= nr_cons) return;
  current = cons_line;
  curcons = &cons_table[cons_line];
  set_6845(VID_ORG, curcons->c_org);
  set_6845(CURSOR, curcons->c_cur);
}


/*===========================================================================*
 *				con_loadfont				     *
 *===========================================================================*/
PUBLIC int con_loadfont(phys_bytes user_phys)
{
#if 0
	/* Load a font into the EGA or VGA adapter. */
  static struct sequence seq1[7] = {
	{ GA_SEQUENCER_INDEX, 0x00, 0x01 },
	{ GA_SEQUENCER_INDEX, 0x02, 0x04 },
	{ GA_SEQUENCER_INDEX, 0x04, 0x07 },
	{ GA_SEQUENCER_INDEX, 0x00, 0x03 },
	{ GA_GRAPHICS_INDEX, 0x04, 0x02 },
	{ GA_GRAPHICS_INDEX, 0x05, 0x00 },
	{ GA_GRAPHICS_INDEX, 0x06, 0x00 },
  };
  static struct sequence seq2[7] = {
	{ GA_SEQUENCER_INDEX, 0x00, 0x01 },
	{ GA_SEQUENCER_INDEX, 0x02, 0x03 },
	{ GA_SEQUENCER_INDEX, 0x04, 0x03 },
	{ GA_SEQUENCER_INDEX, 0x00, 0x03 },
	{ GA_GRAPHICS_INDEX, 0x04, 0x00 },
	{ GA_GRAPHICS_INDEX, 0x05, 0x10 },
	{ GA_GRAPHICS_INDEX, 0x06,    0 },
  };

  seq2[6].value= color ? 0x0E : 0x0A;

  if (!ega) return(ENOTTY);

  lock();
  ga_program(seq1);	/* bring font memory into view */

  phys_copy(user_phys, (phys_bytes)GA_VIDEO_ADDRESS, (phys_bytes)GA_FONT_SIZE);

  ga_program(seq2);	/* restore */
  unlock();
#endif

  return(OK);
}


/*===========================================================================*
 *				ga_program				     *
 *===========================================================================*/
#if 0
PRIVATE void ga_program(seq)
struct sequence *seq;
{
  int len= 7;
  do {
	outb(seq->index, seq->port);
	outb(seq->index+1, seq->value);
	seq++;
  } while (--len > 0);
}
#endif

/*===========================================================================*
 *				cons_ioctl				     *
 *===========================================================================*/
PRIVATE void cons_ioctl(tp)
tty_t *tp;
{
/* Set the screen dimensions. */

  tp->tty_winsize.ws_row= scr_lines;
  tp->tty_winsize.ws_col= scr_width;
  tp->tty_winsize.ws_xpixel= scr_width * 8;
  tp->tty_winsize.ws_ypixel= scr_lines * font_lines;
}

/*
 * Copy count characters from kernel memory to video memory.  Src is an ordinary
 * pointer to a word, but dst and count are character (word) based video offset
 * and count.  If src is null then screen memory is blanked by filling it with
 * blank_color.
 */
static void mem_vid_copy(u16_t *src, unsigned dst, unsigned count)
{
	for (int i=0;i<count;i++){
		if (src == BLANK_MEM)
			*((u16_t*)vid_base + dst + i) = blank_color;
		else
			*((u16_t*)vid_base + dst + i) = *(src + i);
	}
}

/*
 * Copy count characters from video memory to video memory.  Handle overlap.
 * Used for scrolling, line or character insertion and deletion.  Src, dst
 * and count are character (word) based video offsets and count.
 */
static void vid_vid_copy(unsigned src, unsigned dst, unsigned count)
{
	for (int i=0;i<count;i++){
		*((u16_t*)vid_base + dst + i) = *((u16_t*)vid_base + src + i);
	}

}
/*
 * Copy count characters from video memory to kernel memory.
 */

static void vid_mem_copy(unsigned src, u16_t * dst, unsigned count)
{
	for (int i=0;i<count;i++){
		*(dst + i) = *((u16_t*)vid_base + src + i);
	}
}

