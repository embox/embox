/* $Id: nano.h 4508 2010-06-21 03:10:10Z astyanax $ */
/**************************************************************************
 *   nano.h                                                               *
 *                                                                        *
 *   Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,  *
 *   2008, 2009 Free Software Foundation, Inc.                            *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 3, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful, but  *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *   General Public License for more details.                             *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA            *
 *   02110-1301, USA.                                                     *
 *                                                                        *
 **************************************************************************/

#ifndef NANO_H
#define NANO_H 1

//#ifdef HAVE_CONFIG_H
#include "config.h"
//#endif

#ifdef NEED_XOPEN_SOURCE_EXTENDED
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1
#endif /* _XOPEN_SOURCE_EXTENDED */
#endif /* NEED_XOPEN_SOURCE_EXTENDED */

#ifdef __TANDEM
/* Tandem NonStop Kernel support. */
#include <floss.h>
#define NANO_ROOT_UID 65535
#else
#define NANO_ROOT_UID 0
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* Suppress warnings for __attribute__((warn_unused_result)) */
#define IGNORE_CALL_RESULT(call) do { if (call) {} } while(0)

/* Macros for flags. */
#define FLAGOFF(flag) ((flag) / (sizeof(unsigned) * 8))
#define FLAGMASK(flag) (1 << ((flag) % (sizeof(unsigned) * 8)))
#define FLAGS(flag) flags[FLAGOFF(flag)]
#define SET(flag) FLAGS(flag) |= FLAGMASK(flag)
#define UNSET(flag) FLAGS(flag) &= ~FLAGMASK(flag)
#define ISSET(flag) ((FLAGS(flag) & FLAGMASK(flag)) != 0)
#define TOGGLE(flag) FLAGS(flag) ^= FLAGMASK(flag)

/* Macros for character allocation and more. */
#define charalloc(howmuch) (char *)nmalloc((howmuch) * sizeof(char))
#define charealloc(ptr, howmuch) (char *)nrealloc(ptr, (howmuch) * sizeof(char))
#define charmove(dest, src, n) memmove(dest, src, (n) * sizeof(char))
#define charset(dest, src, n) memset(dest, src, (n) * sizeof(char))

/* Set a default value for PATH_MAX if there isn't one. */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef USE_SLANG
/* Slang support. */
#include <slcurses.h>
/* Slang curses emulation brain damage, part 3: Slang doesn't define the
 * curses equivalents of the Insert or Delete keys. */
#define KEY_DC SL_KEY_DELETE
#define KEY_IC SL_KEY_IC
/* Ncurses support. */
#elif defined(HAVE_NCURSES_H)
#include <ncurses.h>
#else
/* Curses support. */
#include <curses.h>
#endif /* CURSES_H */

#ifdef ENABLE_NLS
/* Native language support. */
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif
#define _(string) gettext(string)
#define P_(singular, plural, number) ngettext(singular, plural, number)
#else
#define _(string) (string)
#define P_(singular, plural, number) (number == 1 ? singular : plural)
#endif
#define gettext_noop(string) (string)
#define N_(string) gettext_noop(string)
	/* Mark a string that will be sent to gettext() later. */

#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#ifndef NANO_TINY
#include <setjmp.h>
#endif
#include <assert.h>

/* If no vsnprintf(), use the version from glib 2.x. */
#ifndef HAVE_VSNPRINTF
#include <glib.h>
#define vsnprintf g_vsnprintf
#endif

/* If no isblank(), iswblank(), strcasecmp(), strncasecmp(),
 * strcasestr(), strnlen(), getdelim(), or getline(), use the versions
 * we have. */
#ifndef HAVE_ISBLANK
#define isblank nisblank
#endif
#ifndef HAVE_ISWBLANK
#define iswblank niswblank
#endif
#ifndef HAVE_STRCASECMP
#define strcasecmp nstrcasecmp
#endif
#ifndef HAVE_STRNCASECMP
#define strncasecmp nstrncasecmp
#endif
#ifndef HAVE_STRCASESTR
#define strcasestr nstrcasestr
#endif
#ifndef HAVE_STRNLEN
#define strnlen nstrnlen
#endif
#ifndef HAVE_GETDELIM
#define getdelim ngetdelim
#endif
#ifndef HAVE_GETLINE
#define getline ngetline
#endif

/* If we aren't using ncurses with mouse support, turn the mouse support
 * off, as it's useless then. */
#ifndef NCURSES_MOUSE_VERSION
#define DISABLE_MOUSE 1
#endif

#if defined(DISABLE_WRAPPING) && defined(DISABLE_JUSTIFY)
#define DISABLE_WRAPJUSTIFY 1
#endif

/* Enumeration types. */
typedef enum {
    NIX_FILE, DOS_FILE, MAC_FILE
} file_format;

typedef enum {
    OVERWRITE, APPEND, PREPEND
} append_type;

typedef enum {
    UP_DIR, DOWN_DIR
} scroll_dir;

typedef enum {
    CENTER, NONE
} update_type;

typedef enum {
    CONTROL, META, FKEY, RAW
}  function_type;

typedef enum {
    ADD, DEL, REPLACE, SPLIT, UNSPLIT, CUT, UNCUT, ENTER, INSERT, OTHER
} undo_type;

#ifdef ENABLE_COLOR
typedef struct colortype {
    short fg;
	/* This syntax's foreground color. */
    short bg;
	/* This syntax's background color. */
    bool bright;
	/* Is this color A_BOLD? */
    bool icase;
	/* Is this regex string case insensitive? */
    int pairnum;
	/* The color pair number used for this foreground color and
	 * background color. */
    char *start_regex;
	/* The start (or all) of the regex string. */
    regex_t *start;
	/* The compiled start (or all) of the regex string. */
    char *end_regex;
	/* The end (if any) of the regex string. */
    regex_t *end;
	/* The compiled end (if any) of the regex string. */
    struct colortype *next;
	/* Next set of colors. */
     int id;
	/* basic id for assigning to lines later */
} colortype;

typedef struct exttype {
    char *ext_regex;
	/* The extensions that match this syntax. */
    regex_t *ext;
	/* The compiled extensions that match this syntax. */
    struct exttype *next;
	/* Next set of extensions. */
} exttype;

typedef struct syntaxtype {
    char *desc;
	/* The name of this syntax. */
    exttype *extensions;
	/* The list of extensions that this syntax applies to. */
    exttype *headers;
	/* Regexes to match on the 'header' (1st line) of the file */
    colortype *color;
	/* The colors used in this syntax. */
    int nmultis;
	/* How many multi line strings this syntax has */
    struct syntaxtype *next;
	/* Next syntax. */
} syntaxtype;

#define CNONE 		(1<<1)
	/* Yay, regex doesn't apply to this line at all! */
#define CBEGINBEFORE 	(1<<2)
	/* regex starts on an earlier line, ends on this one */
#define CENDAFTER 	(1<<3)
	/* regex sraers on this line and ends on a later one */
#define CWHOLELINE 	(1<<4)
	/* whole line engulfed by the regex  start < me, end > me */
#define CSTARTENDHERE 	(1<<5)
	/* regex starts and ends within this line */
#define CWTF		(1<<6)
	/* Something else */

#endif /* ENABLE_COLOR */


/* Structure types. */
typedef struct filestruct {
    char *data;
	/* The text of this line. */
    ssize_t lineno;
	/* The number of this line. */
    struct filestruct *next;
	/* Next node. */
    struct filestruct *prev;
	/* Previous node. */
#ifdef ENABLE_COLOR
    short *multidata;		/* Array of which multi-line regexes apply to this line */
#endif
} filestruct;

typedef struct partition {
    filestruct *fileage;
	/* The top line of this portion of the file. */
    filestruct *top_prev;
	/* The line before the top line of this portion of the file. */
    char *top_data;
	/* The text before the beginning of the top line of this portion
	 * of the file. */
    filestruct *filebot;
	/* The bottom line of this portion of the file. */
    filestruct *bot_next;
	/* The line after the bottom line of this portion of the
	 * file. */
    char *bot_data;
	/* The text after the end of the bottom line of this portion of
	 * the file. */
} partition;

#ifndef NANO_TINY
typedef struct undo {
    ssize_t lineno;
    undo_type type;
	/* What type of undo was this */
    int begin;
	/* Where did this  action begin or end */
    char *strdata;
	/* String type data we will use for ccopying the affected line back */
    char *strdata2;
	/* Sigh, need this too it looks like */
    int xflags;
	/* Some flag data we need */

    /* Cut specific stuff we need */
    filestruct *cutbuffer;
	/* Copy of the cutbuffer */
    filestruct *cutbottom;
	/* Copy of cutbottom */
    bool mark_set;
	/* was the marker set when we cut */
    bool to_end;
	/* was this a cut to end */
    ssize_t mark_begin_lineno;
	/* copy copy copy */
    ssize_t mark_begin_x;
	/* Another shadow variable */
    struct undo *next;
} undo;


#endif /* NANO_TINY */


typedef struct openfilestruct {
    char *filename;
	/* The current file's name. */
    filestruct *fileage;
	/* The current file's first line. */
    filestruct *filebot;
	/* The current file's last line. */
    filestruct *edittop;
	/* The current top of the edit window. */
    filestruct *current;
	/* The current file's current line. */
    size_t totsize;
	/* The current file's total number of characters. */
    size_t current_x;
	/* The current file's x-coordinate position. */
    size_t placewewant;
	/* The current file's place we want. */
    ssize_t current_y;
	/* The current file's y-coordinate position. */
    bool modified;
	/* Whether the current file has been modified. */
#ifndef NANO_TINY
    bool mark_set;
	/* Whether the mark is on in the current file. */
    filestruct *mark_begin;
	/* The current file's beginning marked line, if any. */
    size_t mark_begin_x;
	/* The current file's beginning marked line's x-coordinate
	 * position, if any. */
    file_format fmt;
	/* The current file's format. */
    struct stat *current_stat;
	/* The current file's stat. */
    undo *undotop;
	/* Top of the undo list */
    undo *current_undo;
	/* The current (i.e. n ext) level of undo */
    undo_type last_action;
#endif
#ifdef ENABLE_COLOR
    syntaxtype *syntax;
	/* The  syntax struct for this file, if any */
    colortype *colorstrings;
	/* The current file's associated colors. */
#endif
    struct openfilestruct *next;
	/* Next node. */
    struct openfilestruct *prev;
	/* Previous node. */
} openfilestruct;

typedef struct shortcut {
    const char *desc;
	/* The function's description, e.g. "Page Up". */
#ifndef DISABLE_HELP
    const char *help;
	/* The help file entry text for this function. */
    bool blank_after;
	/* Whether there should be a blank line after the help entry
	 * text for this function. */
#endif
    /* Note: Key values that aren't used should be set to
     * NANO_NO_KEY. */
    int ctrlval;
	/* The special sentinel key or control key we want bound, if
	 * any. */
    int metaval;
	/* The meta key we want bound, if any. */
    int funcval;
	/* The function key we want bound, if any. */
    int miscval;
	/* The other meta key we want bound, if any. */
    bool viewok;
	/* Is this function allowed when in view mode? */
    void (*func)(void);
	/* The function to call when we get this key. */
    struct shortcut *next;
	/* Next shortcut. */
} shortcut;

#ifdef ENABLE_NANORC
typedef struct rcoption {
   const char *name;
	/* The name of the rcfile option. */
   long flag;
	/* The flag associated with it, if any. */
} rcoption;

#endif

typedef struct sc {
    char *keystr;
	/* The shortcut key for a function, ASCII version */
    function_type type;
        /* What kind of function key is it for convenience later */
    int seq;
        /* The actual sequence to check on the the type is determined */
    int menu;
        /* What list does this apply to */
    short scfunc;
        /* The function we're going to run */
    int toggle;
        /* If a toggle, what we're toggling */
    bool execute;
	/* Whether to execute the function in question or just return
	   so the sequence can be caught by the calling code */
    struct sc *next;
        /* Next in the list */
} sc;

typedef struct subnfunc {
    short scfunc;
	/* What function is this */
    int menus;
	/* In what menus does this function applu */
    const char *desc;
	/* The function's description, e.g. "Page Up". */
#ifndef DISABLE_HELP
    const char *help;
	/* The help file entry text for this function. */
    bool blank_after;
	/* Whether there should be a blank line after the help entry
	 * text for this function. */
#endif
    bool viewok;
        /* Is this function allowed when in view mode? */
    long toggle;
	/* If this is a toggle, if nonzero what toggle to set */
    struct subnfunc *next;
	/* next item in the list */
} subnfunc;


/* Enumeration to be used in flags table. See FLAGBIT and FLAGOFF
 * definitions. */
enum
{
    DONTUSE,
    CASE_SENSITIVE,
    CONST_UPDATE,
    NO_HELP,
    NOFOLLOW_SYMLINKS,
    SUSPEND,
    NO_WRAP,
    AUTOINDENT,
    VIEW_MODE,
    USE_MOUSE,
    USE_REGEXP,
    TEMP_FILE,
    CUT_TO_END,
    BACKWARDS_SEARCH,
    MULTIBUFFER,
    SMOOTH_SCROLL,
    REBIND_DELETE,
    REBIND_KEYPAD,
    NO_CONVERT,
    BACKUP_FILE,
    INSECURE_BACKUP,
    NO_COLOR_SYNTAX,
    PRESERVE,
    HISTORYLOG,
    RESTRICTED,
    SMART_HOME,
    WHITESPACE_DISPLAY,
    MORE_SPACE,
    TABS_TO_SPACES,
    QUICK_BLANK,
    WORD_BOUNDS,
    NO_NEWLINES,
    BOLD_TEXT,
    QUIET,
    UNDOABLE,
    SOFTWRAP
};

/* Flags for which menus in which a given function should be present */
#define MMAIN				(1<<0)
#define	MWHEREIS			(1<<1)
#define	MREPLACE			(1<<2)
#define	MREPLACE2			(1<<3)
#define	MGOTOLINE			(1<<4)
#define	MWRITEFILE			(1<<5)
#define	MINSERTFILE			(1<<6)
#define	MEXTCMD				(1<<7)
#define	MHELP				(1<<8)
#define	MSPELL				(1<<9)
#define	MBROWSER			(1<<10)
#define	MWHEREISFILE			(1<<11)
#define MGOTODIR			(1<<12)
#define MYESNO				(1<<13)
/* This really isnt all but close enough */
#define	MALL				(MMAIN|MWHEREIS|MREPLACE|MREPLACE2|MGOTOLINE|MWRITEFILE|MINSERTFILE|MEXTCMD|MSPELL|MBROWSER|MWHEREISFILE|MGOTODIR|MHELP)

/* Control key sequences.  Changing these would be very, very bad. */
#define NANO_CONTROL_SPACE 0
#define NANO_CONTROL_A 1
#define NANO_CONTROL_B 2
#define NANO_CONTROL_C 3
#define NANO_CONTROL_D 4
#define NANO_CONTROL_E 5
#define NANO_CONTROL_F 6
#define NANO_CONTROL_G 7
#define NANO_CONTROL_H 8
#define NANO_CONTROL_I 9
#define NANO_CONTROL_J 10
#define NANO_CONTROL_K 11
#define NANO_CONTROL_L 12
#define NANO_CONTROL_M 13
#define NANO_CONTROL_N 14
#define NANO_CONTROL_O 15
#define NANO_CONTROL_P 16
#define NANO_CONTROL_Q 17
#define NANO_CONTROL_R 18
#define NANO_CONTROL_S 19
#define NANO_CONTROL_T 20
#define NANO_CONTROL_U 21
#define NANO_CONTROL_V 22
#define NANO_CONTROL_W 23
#define NANO_CONTROL_X 24
#define NANO_CONTROL_Y 25
#define NANO_CONTROL_Z 26
#define NANO_CONTROL_3 27
#define NANO_CONTROL_4 28
#define NANO_CONTROL_5 29
#define NANO_CONTROL_6 30
#define NANO_CONTROL_7 31
#define NANO_CONTROL_8 127

/* Meta key sequences. */
#define NANO_META_SPACE ' '
#define NANO_META_LPARENTHESIS '('
#define NANO_META_RPARENTHESIS ')'
#define NANO_META_PLUS '+'
#define NANO_META_COMMA ','
#define NANO_META_MINUS '-'
#define NANO_META_PERIOD '.'
#define NANO_META_SLASH '/'
#define NANO_META_0 '0'
#define NANO_META_6 '6'
#define NANO_META_9 '9'
#define NANO_META_LCARET '<'
#define NANO_META_EQUALS '='
#define NANO_META_RCARET '>'
#define NANO_META_QUESTION '?'
#define NANO_META_BACKSLASH '\\'
#define NANO_META_RBRACKET ']'
#define NANO_META_CARET '^'
#define NANO_META_UNDERSCORE '_'
#define NANO_META_A 'a'
#define NANO_META_B 'b'
#define NANO_META_C 'c'
#define NANO_META_D 'd'
#define NANO_META_E 'e'
#define NANO_META_F 'f'
#define NANO_META_G 'g'
#define NANO_META_H 'h'
#define NANO_META_I 'i'
#define NANO_META_J 'j'
#define NANO_META_K 'k'
#define NANO_META_L 'l'
#define NANO_META_M 'm'
#define NANO_META_N 'n'
#define NANO_META_O 'o'
#define NANO_META_P 'p'
#define NANO_META_Q 'q'
#define NANO_META_R 'r'
#define NANO_META_S 's'
#define NANO_META_T 't'
#define NANO_META_U 'u'
#define NANO_META_V 'v'
#define NANO_META_W 'w'
#define NANO_META_X 'x'
#define NANO_META_Y 'y'
#define NANO_META_Z 'z'
#define NANO_META_LCURLYBRACKET '{'
#define NANO_META_PIPE '|'
#define NANO_META_RCURLYBRACKET '}'

/* Some semi-changeable keybindings; don't play with these unless you're
 * sure you know what you're doing.  Assume ERR is defined as -1. */

/* No key at all. */
#define NANO_NO_KEY			-2

/* Normal keys. */
#define NANO_XON_KEY			NANO_CONTROL_Q
#define NANO_XOFF_KEY			NANO_CONTROL_S
#define NANO_CANCEL_KEY			NANO_CONTROL_C
#define NANO_EXIT_KEY			NANO_CONTROL_X
#define NANO_EXIT_FKEY			KEY_F(2)
#define NANO_INSERTFILE_KEY		NANO_CONTROL_R
#define NANO_INSERTFILE_FKEY		KEY_F(5)
#define NANO_TOOTHERINSERT_KEY		NANO_CONTROL_X
#define NANO_WRITEOUT_KEY		NANO_CONTROL_O
#define NANO_WRITEOUT_FKEY		KEY_F(3)
#define NANO_GOTOLINE_KEY		NANO_CONTROL_7
#define NANO_GOTOLINE_FKEY		KEY_F(13)
#define NANO_GOTOLINE_METAKEY		NANO_META_G
#define NANO_GOTODIR_KEY		NANO_CONTROL_7
#define NANO_GOTODIR_FKEY		KEY_F(13)
#define NANO_GOTODIR_METAKEY		NANO_META_G
#define NANO_TOGOTOLINE_KEY		NANO_CONTROL_T
#define NANO_HELP_KEY			NANO_CONTROL_G
#define NANO_HELP_FKEY			KEY_F(1)
#define NANO_WHEREIS_KEY		NANO_CONTROL_W
#define NANO_WHEREIS_FKEY		KEY_F(6)
#define NANO_WHEREIS_NEXT_KEY		NANO_META_W
#define NANO_WHEREIS_NEXT_FKEY		KEY_F(16)
#define NANO_TOOTHERWHEREIS_KEY		NANO_CONTROL_T
#define NANO_REGEXP_KEY			NANO_META_R
#define NANO_REPLACE_KEY		NANO_CONTROL_4
#define NANO_REPLACE_FKEY		KEY_F(14)
#define NANO_REPLACE_METAKEY		NANO_META_R
#define NANO_TOOTHERSEARCH_KEY		NANO_CONTROL_R
#define NANO_PREVPAGE_KEY		NANO_CONTROL_Y
#define NANO_PREVPAGE_FKEY		KEY_F(7)
#define NANO_NEXTPAGE_KEY		NANO_CONTROL_V
#define NANO_NEXTPAGE_FKEY		KEY_F(8)
#define NANO_CUT_KEY			NANO_CONTROL_K
#define NANO_CUT_FKEY			KEY_F(9)
#define NANO_COPY_KEY			NANO_META_CARET
#define NANO_COPY_METAKEY		NANO_META_6
#define NANO_UNCUT_KEY			NANO_CONTROL_U
#define NANO_UNCUT_FKEY			KEY_F(10)
#define NANO_CURSORPOS_KEY		NANO_CONTROL_C
#define NANO_CURSORPOS_FKEY		KEY_F(11)
#define NANO_SPELL_KEY			NANO_CONTROL_T
#define NANO_SPELL_FKEY			KEY_F(12)
#define NANO_FIRSTLINE_KEY		NANO_PREVPAGE_KEY
#define NANO_FIRSTLINE_FKEY		NANO_PREVPAGE_FKEY
#define NANO_FIRSTLINE_METAKEY		NANO_META_BACKSLASH
#define NANO_FIRSTLINE_METAKEY2		NANO_META_PIPE
#define NANO_FIRSTFILE_KEY		NANO_FIRSTLINE_KEY
#define NANO_FIRSTFILE_FKEY		NANO_FIRSTLINE_FKEY
#define NANO_FIRSTFILE_METAKEY		NANO_FIRSTLINE_METAKEY
#define NANO_FIRSTFILE_METAKEY2		NANO_FIRSTLINE_METAKEY2
#define NANO_LASTLINE_KEY		NANO_NEXTPAGE_KEY
#define NANO_LASTLINE_FKEY		NANO_NEXTPAGE_FKEY
#define NANO_LASTLINE_METAKEY		NANO_META_SLASH
#define NANO_LASTLINE_METAKEY2		NANO_META_QUESTION
#define NANO_LASTFILE_KEY		NANO_LASTLINE_KEY
#define NANO_LASTFILE_FKEY		NANO_LASTLINE_FKEY
#define NANO_LASTFILE_METAKEY		NANO_LASTLINE_METAKEY
#define NANO_LASTFILE_METAKEY2		NANO_LASTLINE_METAKEY2
#define NANO_REFRESH_KEY		NANO_CONTROL_L
#define NANO_JUSTIFY_KEY		NANO_CONTROL_J
#define NANO_JUSTIFY_FKEY		KEY_F(4)
#define NANO_UNJUSTIFY_KEY		NANO_UNCUT_KEY
#define NANO_UNJUSTIFY_FKEY		NANO_UNCUT_FKEY
#define NANO_PREVLINE_KEY		NANO_CONTROL_P
#define NANO_NEXTLINE_KEY		NANO_CONTROL_N
#define NANO_FORWARD_KEY		NANO_CONTROL_F
#define NANO_BACK_KEY			NANO_CONTROL_B
#define NANO_MARK_KEY			NANO_CONTROL_6
#define NANO_MARK_METAKEY		NANO_META_A
#define NANO_MARK_FKEY			KEY_F(15)
#define NANO_HOME_KEY			NANO_CONTROL_A
#define NANO_END_KEY			NANO_CONTROL_E
#define NANO_DELETE_KEY			NANO_CONTROL_D
#define NANO_BACKSPACE_KEY		NANO_CONTROL_H
#define NANO_TAB_KEY			NANO_CONTROL_I
#define NANO_INDENT_KEY			NANO_META_RCURLYBRACKET
#define NANO_UNINDENT_KEY		NANO_META_LCURLYBRACKET
#define NANO_SUSPEND_KEY		NANO_CONTROL_Z
#define NANO_ENTER_KEY			NANO_CONTROL_M
#define NANO_TOFILES_KEY		NANO_CONTROL_T
#define NANO_APPEND_KEY			NANO_META_A
#define NANO_PREPEND_KEY		NANO_META_P
#define NANO_PREVFILE_KEY		NANO_META_LCARET
#define NANO_PREVFILE_METAKEY		NANO_META_COMMA
#define NANO_NEXTFILE_KEY		NANO_META_RCARET
#define NANO_NEXTFILE_METAKEY		NANO_META_PERIOD
#define NANO_BRACKET_KEY		NANO_META_RBRACKET
#define NANO_NEXTWORD_KEY		NANO_CONTROL_SPACE
#define NANO_PREVWORD_KEY		NANO_META_SPACE
#define NANO_WORDCOUNT_KEY		NANO_META_D
#define NANO_SCROLLUP_KEY		NANO_META_MINUS
#define NANO_SCROLLDOWN_KEY		NANO_META_PLUS
#define NANO_SCROLLUP_METAKEY		NANO_META_UNDERSCORE
#define NANO_SCROLLDOWN_METAKEY		NANO_META_EQUALS
#define NANO_CUTTILLEND_METAKEY		NANO_META_T
#define NANO_PARABEGIN_KEY		NANO_CONTROL_W
#define NANO_PARABEGIN_METAKEY		NANO_META_LPARENTHESIS
#define NANO_PARABEGIN_METAKEY2		NANO_META_9
#define NANO_PARAEND_KEY		NANO_CONTROL_O
#define NANO_PARAEND_METAKEY		NANO_META_RPARENTHESIS
#define NANO_PARAEND_METAKEY2		NANO_META_0
#define NANO_FULLJUSTIFY_KEY		NANO_CONTROL_U
#define NANO_FULLJUSTIFY_METAKEY	NANO_META_J
#define NANO_VERBATIM_KEY		NANO_META_V

/* Toggles do not exist if NANO_TINY is defined. */
#ifndef NANO_TINY

/* No toggle at all. */
#define TOGGLE_NO_KEY			-2

/* Normal toggles. */
#define TOGGLE_NOHELP_KEY		NANO_META_X
#define TOGGLE_CONST_KEY		NANO_META_C
#define TOGGLE_MORESPACE_KEY		NANO_META_O
#define TOGGLE_SMOOTH_KEY		NANO_META_S
#define TOGGLE_WHITESPACE_KEY		NANO_META_P
#define TOGGLE_SYNTAX_KEY		NANO_META_Y
#define TOGGLE_SMARTHOME_KEY		NANO_META_H
#define TOGGLE_AUTOINDENT_KEY		NANO_META_I
#define TOGGLE_CUTTOEND_KEY		NANO_META_K
#define TOGGLE_WRAP_KEY			NANO_META_L
#define TOGGLE_TABSTOSPACES_KEY		NANO_META_Q
#define TOGGLE_BACKUP_KEY		NANO_META_B
#define TOGGLE_MULTIBUFFER_KEY		NANO_META_F
#define TOGGLE_MOUSE_KEY		NANO_META_M
#define TOGGLE_NOCONVERT_KEY		NANO_META_N
#define TOGGLE_SUSPEND_KEY		NANO_META_Z
#define TOGGLE_CASE_KEY			NANO_META_C
#define TOGGLE_BACKWARDS_KEY		NANO_META_B
#define TOGGLE_DOS_KEY			NANO_META_D
#define TOGGLE_MAC_KEY			NANO_META_M

/* Extra bits for the undo function */
#define UNDO_DEL_DEL		(1<<0)
#define UNDO_DEL_BACKSPACE	(1<<1)
#define UNDO_SPLIT_MADENEW	(1<<2)

/* Since in ISO C you can't pass around function pointers anymore,
  let's make some integer macros for function names, and then I
  can go cut my wrists after writing the big switch statement
  that will necessitate. */

#endif /* !NANO_TINY */

#define CASE_SENS_MSG 1
#define BACKWARDS_MSG 2
#define REGEXP_MSG 3
#define WHEREIS_NEXT_MSG 4
#define FIRST_FILE_MSG 5
#define LAST_FILE_MSG 6
#define GOTO_DIR_MSG 7
#define TOTAL_REFRESH 8
#define DO_HELP_VOID 9
#define DO_SEARCH 10
#define DO_PAGE_UP 11
#define DO_PAGE_DOWN 12
#define DO_UP_VOID 13
#define DO_LEFT 14
#define DO_DOWN_VOID 15
#define DO_RIGHT 16
#define DO_ENTER 17
#define DO_EXIT 18
#define NEW_BUFFER_MSG 19
#define EXT_CMD_MSG 20
#define TO_FILES_MSG 21
#define DOS_FORMAT_MSG 23
#define MAC_FORMAT_MSG 24
#define BACKUP_FILE_MSG 25
#define PREPEND_MSG 26
#define APPEND_MSG 27
#define DO_FIRST_LINE 28
#define DO_LAST_LINE 29
#define DO_TOGGLE 30
#define GOTOTEXT_MSG 31
#define NO_REPLACE_MSG 32
#define DO_BACKSPACE 33
#define DO_DELETE 34
#define DO_TAB 35
#define DO_VERBATIM_INPUT 36
#define SWITCH_TO_NEXT_BUFFER_VOID 37
#define SWITCH_TO_PREV_BUFFER_VOID 38
#define DO_END 39
#define DO_HOME 40
#define NEXT_HISTORY_MSG 41
#define PREV_HISTORY_MSG 42
#define DO_REDO 43
#define DO_UNDO 44
#define DO_WORDLINECHAR_COUNT 45
#define DO_FIND_BRACKET 46
#define DO_PREV_WORD_VOID 47
#define DO_SUSPEND_VOID 48
#define CANCEL_MSG 49
#define DO_WRITEOUT_VOID 50
#define DO_INSERTFILE_VOID 51
#define DO_CUT_TEXT_VOID 52
#define DO_UNCUT_TEXT 53
#define DO_CURSORPOS_VOID 54
#define DO_GOTOLINECOLUMN_VOID 55
#define DO_REPLACE 56
#define DO_JUSTIFY_VOID 57
#define DO_PARA_BEGIN_VOID 58
#define DO_PARA_END_VOID 59
#define DO_FULL_JUSTIFY 60
#define DO_MARK 61
#define DO_RESEARCH 62
#define DO_COPY_TEXT 63
#define DO_INDENT_VOID 64
#define DO_UNINDENT 65
#define DO_SCROLL_UP 66
#define DO_SCROLL_DOWN 67
#define DO_NEXT_WORD_VOID 68
#define DO_CUT_TILL_END 69
#define NANO_GOTODIR_MSG 70
#define NANO_LASTFILE_MSG 71
#define NANO_FIRSTFILE_MSG 72
#define INSERT_FILE_MSG 73
#define NANO_MULTIBUFFER_MSG 74
#define NANO_EXECUTE_MSG 75
#define NANO_BACKUP_MSG 76
#define NANO_PREPEND_MSG 77
#define NANO_APPEND_MSG 78
#define NANO_MAC_MSG 79
#define NANO_DOS_MSG 80
#define NANO_TOFILES_MSG 81
#define NANO_NEXT_HISTORY_MSG 82
#define NANO_PREV_HISTORY_MSG 83
#define NANO_REGEXP_MSG 84
#define NANO_REVERSE_MSG 85
#define NANO_CASE_MSG 86
#define NANO_SUSPEND_MSG 87
#define SUSPEND_MSG 88
#define NANO_REFRESH_MSG 89
#define REFRESH_MSG 90
#define NANO_WORDCOUNT_MSG 91
#define NANO_FULLJUSTIFY_MSG 92
#define FULLJSTIFY_MSG 93
#define XOFF_COMPLAINT 94
#define XON_COMPLAINT 95
#define NANO_CUT_TILL_END_MSG 96
#define NANO_BACKSPACE_MSG 97
#define NANO_DELETE_MSG 98
#define NANO_ENTER_MSG 99
#define NANO_TAB_MSG 100
#define NANO_VERBATIM_MSG 101
#define NANO_NEXTFILE_MSG 102
#define NANO_PREVFILE_MSG 103
#define NANO_SCROLLDOWN_MSG 104
#define NANO_SCROLLUP_MSG 105
#define NANO_BRACKET_MSG 106
#define NANO_PARAEND_MSG 107
#define END_OF_PAR_MSG 108
#define NANO_PARABEGIN_MSG 109
#define BEG_OF_PAR_MSG 110
#define NANO_END_MSG 111
#define NANO_HOME_MSG 112
#define NANO_NEXTLINE_MSG 113
#define NANO_PREVLINE_MSG 114
#define NANO_PREVWORD_MSG 115
#define NANO_NEXTWORD_MSG 116
#define NANO_BACK_MSG 117
#define NANO_FORWARD_MSG 118
#define NANO_REDO_MSG 119
#define NANO_UNDO_MSG 120
#define NANO_UNINDENT_MSG 121
#define NANO_INDENT_MSG 122
#define NANO_COPY_MSG 123
#define NANO_WHEREIS_NEXT_MSG 124
#define NANO_MARK_MSG 125
#define NANO_REPLACE_MSG 126
#define REPLACE_MSG 127
#define NANO_GOTOLINE_MSG 128
#define NANO_LASTLINE_MSG 129
#define NANO_FIRSTLINE_MSG 130
#define NANO_SPELL_MSG 131
#define DO_SPELL 132
#define NANO_CURSORPOS_MSG 133
#define NANO_UNCUT_MSG 134
#define GET_HELP_MSG 135
#define NANO_HELP_MSG 136
#define NANO_CANCEL_MSG 137
#define NANO_EXIT_MSG 138
#define EXIT_MSG 139
#define NANO_EXITBROWSER_MSG 140
#define NANO_WRITEOUT_MSG 141
#define NANO_DISABLED_MSG 142
#define NANO_INSERT_MSG 143
#define WHEREIS_MSG 144
#define NANO_WHEREIS_MSG 145
#define NANO_PREVPAGE_MSG 146
#define NANO_NEXTPAGE_MSG 147
#define NANO_CUT_MSG 148
#define DO_CUT_TEXT 149
#define DO_NEXT_WORD 150
#define DO_PREV_WORD 151




#define VIEW TRUE
#define NOVIEW FALSE

/* The maximum number of entries displayed in the main shortcut list. */
#define MAIN_VISIBLE 12

/* The minimum editor window columns and rows required for nano to work
 * correctly. */
#define MIN_EDITOR_COLS 4
#define MIN_EDITOR_ROWS 1

/* The default number of characters from the end of the line where
 * wrapping occurs. */
#define CHARS_FROM_EOL 8

/* The default width of a tab in spaces. */
#define WIDTH_OF_TAB 8

/* The maximum number of search/replace history strings saved, not
 * counting the blank lines at their ends. */
#define MAX_SEARCH_HISTORY 100

/* The maximum number of bytes buffered at one time. */
#define MAX_BUF_SIZE 128

#endif /* !NANO_H */
