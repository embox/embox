/**
 * @file
 * @brief
 *
 * @date 08.03.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include "embox.h"
#include "proto.h"

EMBOX_CMD(exec);

/**
 * NANO STATIC
 */
extern bool nano__cut__keep_cutbuffer;
extern int nano__files__get_next_filename__ulmax_digits;
extern bool nano__nano__pager_sig_failed;
extern bool nano__nano__pager_input_aborted;
extern bool nano__nano__term_init__newterm_set;
extern size_t nano__nano__do_input__kbinput_len;
extern int *nano__nano__do_input__kbinput;
extern char *nano__prompt__prompt;
extern size_t nano__prompt__statusbar_x;
extern size_t nano__prompt__statusbar_pww;
extern size_t nano__prompt__old_statusbar_x;
extern size_t nano__prompt__old_pww;
extern bool nano__prompt__reset_statusbar_x;
extern int *nano__prompt__do_statusbar_input__kbinput;
extern size_t nano__prompt__do_statusbar_input__kbinput_len;
extern bool nano__search__search_last_line;
extern char *nano__search__search_init__backupstring;
extern int *nano__winio__key_buffer;
extern size_t nano__winio__key_buffer_len;
extern int nano__winio__statusblank;
extern bool nano__winio__disable_cursorpos;
extern int nano__winio__parse_kbinput__escapes;
extern int nano__winio__parse_kbinput__byte_digits;
extern int nano__winio__get_byte_kbinput__byte_digits;
extern int nano__winio__get_byte_kbinput__byte;

static void init_static_vars(void) {
	nano__cut__keep_cutbuffer = FALSE;
	nano__files__get_next_filename__ulmax_digits = -1;
	nano__nano__pager_sig_failed = FALSE;
	nano__nano__pager_input_aborted = FALSE;
	nano__nano__term_init__newterm_set = FALSE;
	nano__nano__do_input__kbinput_len = 0;
	nano__nano__do_input__kbinput = NULL;
	nano__prompt__prompt = NULL;
	nano__prompt__statusbar_x = (size_t)-1;
	nano__prompt__statusbar_pww = (size_t)-1;
	nano__prompt__old_statusbar_x = (size_t)-1;
	nano__prompt__old_pww = (size_t)-1;
	nano__prompt__reset_statusbar_x = FALSE;
	nano__prompt__do_statusbar_input__kbinput = NULL;
	nano__prompt__do_statusbar_input__kbinput_len = 0;
	nano__search__search_last_line = FALSE;
	nano__search__search_init__backupstring = NULL;
	nano__winio__key_buffer = NULL;
	nano__winio__key_buffer_len = 0;
	nano__winio__statusblank = 0;
	nano__winio__disable_cursorpos = FALSE;
	nano__winio__parse_kbinput__escapes = 0;
	nano__winio__parse_kbinput__byte_digits = 0;
	nano__winio__get_byte_kbinput__byte_digits = 0;
	nano__winio__get_byte_kbinput__byte = 0;
}

static void init_global_vars(void) {
	last_search = NULL;
	last_replace = NULL;
	flags[0] = flags[1] = flags[2] = flags[3] = 0;
	editwinrows = 0;
	maxrows = 0;
	cutbuffer = NULL;
	cutbottom = NULL;
	filepart = NULL;
	openfile = NULL;
	nodelay_mode = FALSE;
	answer = NULL;
	tabsize = -1;
	edit_refresh_needed = NULL;
	sclist = NULL;
	allfuncs = NULL;
	reverse_attr = A_REVERSE;
	homedir = NULL;
}

static int exec(int argc, char **argv) {
	init_static_vars();
	init_global_vars();
	return nano_main(argc, argv);
}
