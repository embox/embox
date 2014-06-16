/* $Id: rcfile.c 4508 2010-06-21 03:10:10Z astyanax $ */
/**************************************************************************
 *   rcfile.c                                                             *
 *                                                                        *
 *   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009   *
 *   Free Software Foundation, Inc.                                       *
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

#include "proto.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#ifdef ENABLE_NANORC

static const rcoption rcopts[] = {
    {"boldtext", BOLD_TEXT},
#ifndef DISABLE_JUSTIFY
    {"brackets", 0},
#endif
    {"const", CONST_UPDATE},
#ifndef DISABLE_WRAPJUSTIFY
    {"fill", 0},
#endif
#ifndef DISABLE_MOUSE
    {"mouse", USE_MOUSE},
#endif
#ifdef ENABLE_MULTIBUFFER
    {"multibuffer", MULTIBUFFER},
#endif
    {"morespace", MORE_SPACE},
    {"nofollow", NOFOLLOW_SYMLINKS},
    {"nohelp", NO_HELP},
    {"nonewlines", NO_NEWLINES},
#ifndef DISABLE_WRAPPING
    {"nowrap", NO_WRAP},
#endif
#ifndef DISABLE_OPERATINGDIR
    {"operatingdir", 0},
#endif
    {"preserve", PRESERVE},
#ifndef DISABLE_JUSTIFY
    {"punct", 0},
    {"quotestr", 0},
#endif
    {"rebinddelete", REBIND_DELETE},
    {"rebindkeypad", REBIND_KEYPAD},
#ifdef HAVE_REGEX_H
    {"regexp", USE_REGEXP},
#endif
#ifndef DISABLE_SPELLER
    {"speller", 0},
#endif
    {"suspend", SUSPEND},
    {"tabsize", 0},
    {"tempfile", TEMP_FILE},
    {"view", VIEW_MODE},
#ifndef NANO_TINY
    {"autoindent", AUTOINDENT},
    {"backup", BACKUP_FILE},
    {"allow_insecure_backup", INSECURE_BACKUP},
    {"backupdir", 0},
    {"backwards", BACKWARDS_SEARCH},
    {"casesensitive", CASE_SENSITIVE},
    {"cut", CUT_TO_END},
    {"historylog", HISTORYLOG},
    {"matchbrackets", 0},
    {"noconvert", NO_CONVERT},
    {"quiet", QUIET},
    {"quickblank", QUICK_BLANK},
    {"smarthome", SMART_HOME},
    {"smooth", SMOOTH_SCROLL},
    {"tabstospaces", TABS_TO_SPACES},
    {"undo", UNDOABLE},
    {"whitespace", 0},
    {"wordbounds", WORD_BOUNDS},
    {"softwrap", SOFTWRAP},
#endif
    {NULL, 0}
};

static bool errors = FALSE;
	/* Whether we got any errors while parsing an rcfile. */
static size_t lineno = 0;
	/* If we did, the line number where the last error occurred. */
static char *nanorc = NULL;
	/* The path to the rcfile we're parsing. */
#ifdef ENABLE_COLOR
static syntaxtype *endsyntax = NULL;
	/* The end of the list of syntaxes. */
static exttype *endheader = NULL;
	/* End of header list */
static colortype *endcolor = NULL;
	/* The end of the color list for the current syntax. */

#endif

/* We have an error in some part of the rcfile.  Print the error message
 * on stderr, and then make the user hit Enter to continue starting
 * nano. */
void rcfile_error(const char *msg, ...)
{
    va_list ap;

    if (ISSET(QUIET))
	return;

    fprintf(stderr, "\n");
    if (lineno > 0) {
	errors = TRUE;
	fprintf(stderr, _("Error in %s on line %lu: "), nanorc, (unsigned long)lineno);
    }

    va_start(ap, msg);
    vfprintf(stderr, _(msg), ap);
    va_end(ap);

    fprintf(stderr, "\n");
}

/* Parse the next word from the string, null-terminate it, and return
 * a pointer to the first character after the null terminator.  The
 * returned pointer will point to '\0' if we hit the end of the line. */
char *parse_next_word(char *ptr)
{
    while (!isblank(*ptr) && *ptr != '\0')
	ptr++;

    if (*ptr == '\0')
	return ptr;

    /* Null-terminate and advance ptr. */
    *ptr++ = '\0';

    while (isblank(*ptr))
	ptr++;

    return ptr;
}

/* Parse an argument, with optional quotes, after a keyword that takes
 * one.  If the next word starts with a ", we say that it ends with the
 * last " of the line.  Otherwise, we interpret it as usual, so that the
 * arguments can contain "'s too. */
char *parse_argument(char *ptr)
{
    const char *ptr_save = ptr;
    char *last_quote = NULL;

    assert(ptr != NULL);

    if (*ptr != '"')
	return parse_next_word(ptr);

    do {
	ptr++;
	if (*ptr == '"')
	    last_quote = ptr;
    } while (*ptr != '\0');

    if (last_quote == NULL) {
	if (*ptr == '\0')
	    ptr = NULL;
	else
	    *ptr++ = '\0';
	rcfile_error(N_("Argument '%s' has an unterminated \""), ptr_save);
    } else {
	*last_quote = '\0';
	ptr = last_quote + 1;
    }
    if (ptr != NULL)
	while (isblank(*ptr))
	    ptr++;
    return ptr;
}

#ifdef ENABLE_COLOR
/* Parse the next regex string from the line at ptr, and return it. */
char *parse_next_regex(char *ptr)
{
    assert(ptr != NULL);

    /* Continue until the end of the line, or a " followed by a space, a
     * blank character, or \0. */
    while ((*ptr != '"' || (!isblank(*(ptr + 1)) &&
	*(ptr + 1) != '\0')) && *ptr != '\0')
	ptr++;

    assert(*ptr == '"' || *ptr == '\0');

    if (*ptr == '\0') {
	rcfile_error(
		N_("Regex strings must begin and end with a \" character"));
	return NULL;
    }

    /* Null-terminate and advance ptr. */
    *ptr++ = '\0';

    while (isblank(*ptr))
	ptr++;

    return ptr;
}

/* Compile the regular expression regex to see if it's valid.  Return
 * TRUE if it is, or FALSE otherwise. */
bool nregcomp(const char *regex, int eflags)
{
    regex_t preg;
    const char *r = fixbounds(regex);
    int rc = regcomp(&preg, r, REG_EXTENDED | eflags);

    if (rc != 0) {
	size_t len = regerror(rc, &preg, NULL, 0);
	char *str = charalloc(len);

	regerror(rc, &preg, str, len);
	rcfile_error(N_("Bad regex \"%s\": %s"), r, str);
	free(str);
    }

    regfree(&preg);
    return (rc == 0);
}

/* Parse the next syntax string from the line at ptr, and add it to the
 * global list of color syntaxes. */
void parse_syntax(char *ptr)
{
    const char *fileregptr = NULL, *nameptr = NULL;
    syntaxtype *tmpsyntax;
    exttype *endext = NULL;
	/* The end of the extensions list for this syntax. */

    assert(ptr != NULL);

    if (*ptr == '\0') {
	rcfile_error(N_("Missing syntax name"));
	return;
    }

    if (*ptr != '"') {
	rcfile_error(
		N_("Regex strings must begin and end with a \" character"));
	return;
    }

    ptr++;

    nameptr = ptr;
    ptr = parse_next_regex(ptr);

    if (ptr == NULL)
	return;

    /* Search for a duplicate syntax name.  If we find one, free it, so
     * that we always use the last syntax with a given name. */
    for (tmpsyntax = syntaxes; tmpsyntax != NULL;
	tmpsyntax = tmpsyntax->next) {
	if (strcmp(nameptr, tmpsyntax->desc) == 0) {
	    syntaxtype *prev_syntax = tmpsyntax;

	    tmpsyntax = tmpsyntax->next;
	    free(prev_syntax);
	    break;
	}
    }

    if (syntaxes == NULL) {
	syntaxes = (syntaxtype *)nmalloc(sizeof(syntaxtype));
	endsyntax = syntaxes;
    } else {
	endsyntax->next = (syntaxtype *)nmalloc(sizeof(syntaxtype));
	endsyntax = endsyntax->next;
#ifdef DEBUG
	fprintf(stderr, "Adding new syntax after first one\n");
#endif
    }

    endsyntax->desc = mallocstrcpy(NULL, nameptr);
    endsyntax->color = NULL;
    endcolor = NULL;
    endheader = NULL;
    endsyntax->extensions = NULL;
    endsyntax->headers = NULL;
    endsyntax->next = NULL;
    endsyntax->nmultis = 0;

#ifdef DEBUG
    fprintf(stderr, "Starting a new syntax type: \"%s\"\n", nameptr);
#endif

    /* The "none" syntax is the same as not having a syntax at all, so
     * we can't assign any extensions or colors to it. */
    if (strcmp(endsyntax->desc, "none") == 0) {
	rcfile_error(N_("The \"none\" syntax is reserved"));
	return;
    }

    /* The default syntax should have no associated extensions. */
    if (strcmp(endsyntax->desc, "default") == 0 && *ptr != '\0') {
	rcfile_error(
		N_("The \"default\" syntax must take no extensions"));
	return;
    }

    /* Now load the extensions into their part of the struct. */
    while (*ptr != '\0') {
	exttype *newext;
	    /* The new extension structure. */

	while (*ptr != '"' && *ptr != '\0')
	    ptr++;

	if (*ptr == '\0')
	    return;

	ptr++;

	fileregptr = ptr;
	ptr = parse_next_regex(ptr);
	if (ptr == NULL)
	    break;

	newext = (exttype *)nmalloc(sizeof(exttype));

	/* Save the extension regex if it's valid. */
	if (nregcomp(fileregptr, REG_NOSUB)) {
	    newext->ext_regex = mallocstrcpy(NULL, fileregptr);
	    newext->ext = NULL;

	    if (endext == NULL)
		endsyntax->extensions = newext;
	    else
		endext->next = newext;
	    endext = newext;
	    endext->next = NULL;
	} else
	    free(newext);
    }
}

int check_bad_binding(sc *s)
{
#define BADLISTLEN 1
    int badtypes[BADLISTLEN] = {META};
    int badseqs[BADLISTLEN] = { 91 };
    int i;

    for (i = 0; i < BADLISTLEN; i++)
        if (s->type == badtypes[i] && s->seq == badseqs[i])
	    return 1;

    return 0;
}

void parse_keybinding(char *ptr)
{
    char *keyptr = NULL, *keycopy = NULL, *funcptr = NULL, *menuptr = NULL;
    sc *s, *newsc;
    int i, menu;

    assert(ptr != NULL);

    if (*ptr == '\0') {
	rcfile_error(N_("Missing key name"));
	return;
    }

    keyptr = ptr;
    ptr = parse_next_word(ptr);
    keycopy = mallocstrcpy(NULL, keyptr);
    for (i = 0; i < strlen(keycopy); i++)
	keycopy[i] = toupper(keycopy[i]);

    if (keycopy[0] != 'M' && keycopy[0] != '^' && keycopy[0] != 'F' && keycopy[0] != 'K') {
	rcfile_error(
		N_("keybindings must begin with \"^\", \"M\", or \"F\""));
	return;
    }

    funcptr = ptr;
    ptr = parse_next_word(ptr);

    if (!strcmp(funcptr, "")) {
	rcfile_error(
		N_("Must specify function to bind key to"));
	return;
    }

    menuptr = ptr;
    ptr = parse_next_word(ptr);

    if (!strcmp(menuptr, "")) {
	rcfile_error(
		/* Note to translators, do not translate the word "all"
		   in the sentence below, everything else is fine */
		N_("Must specify menu to bind key to (or \"all\")"));
	return;
    }

    menu = strtomenu(menuptr);
    newsc = strtosc(menu, funcptr);
    if (newsc == NULL) {
	rcfile_error(
		N_("Could not map name \"%s\" to a function"), funcptr);
	return;
    }

    if (menu < 1) {
	rcfile_error(
		N_("Could not map name \"%s\" to a menu"), menuptr);
	return;
    }


#ifdef DEBUG
    fprintf(stderr, "newsc now address %d, menu func assigned = %d, menu = %d\n",
	&newsc, newsc->scfunc, menu);
#endif


    newsc->keystr = keycopy;
    newsc->menu = menu;
    newsc->type = strtokeytype(newsc->keystr);
    assign_keyinfo(newsc);
#ifdef DEBUG
    fprintf(stderr, "s->keystr = \"%s\"\n", newsc->keystr);
    fprintf(stderr, "s->seq = \"%d\"\n", newsc->seq);
#endif

    if (check_bad_binding(newsc)) {
	rcfile_error(
		N_("Sorry, keystr \"%s\" is an illegal binding"), newsc->keystr);
	return;
    }

    /* now let's have some fun.  Try and delete the other entries
       we found for the same menu, then make this new new
       beginning */
    for (s = sclist; s != NULL; s = s->next) {
        if (((s->menu & newsc->menu)) && s->seq == newsc->seq) {
	    s->menu &= ~newsc->menu;
#ifdef DEBUG
	    fprintf(stderr, "replaced menu entry %d\n", s->menu);
#endif
	}
    }
    newsc->next = sclist;
    sclist = newsc;
}

/* Let user unbind a sequence from a given (or all) menus */
void parse_unbinding(char *ptr)
{
    char *keyptr = NULL, *keycopy = NULL, *menuptr = NULL;
    sc *s;
    int i, menu;

    assert(ptr != NULL);

    if (*ptr == '\0') {
	rcfile_error(N_("Missing key name"));
	return;
    }

    keyptr = ptr;
    ptr = parse_next_word(ptr);
    keycopy = mallocstrcpy(NULL, keyptr);
    for (i = 0; i < strlen(keycopy); i++)
	keycopy[i] = toupper(keycopy[i]);

#ifdef DEBUG
    fprintf(stderr, "Starting unbinding code");
#endif

    if (keycopy[0] != 'M' && keycopy[0] != '^' && keycopy[0] != 'F' && keycopy[0] != 'K') {
	rcfile_error(
		N_("keybindings must begin with \"^\", \"M\", or \"F\""));
	return;
    }

    menuptr = ptr;
    ptr = parse_next_word(ptr);

    if (!strcmp(menuptr, "")) {
	rcfile_error(
		/* Note to translators, do not translate the word "all"
		   in the sentence below, everything else is fine */
		N_("Must specify menu to bind key to (or \"all\")"));
	return;
    }

    menu = strtomenu(menuptr);
    if (menu < 1) {
	rcfile_error(
		N_("Could not map name \"%s\" to a menu"), menuptr);
	return;
    }


#ifdef DEBUG
    fprintf(stderr, "unbinding \"%s\" from menu = %d\n", keycopy, menu);
#endif

    /* Now find the apropriate entries in the menu to delete */
    for (s = sclist; s != NULL; s = s->next) {
        if (((s->menu & menu)) && !strcmp(s->keystr,keycopy)) {
	    s->menu &= ~menu;
#ifdef DEBUG
	    fprintf(stderr, "deleted menu entry %d\n", s->menu);
#endif
	}
    }
}


/* Read and parse additional syntax files. */
void parse_include(char *ptr)
{
    struct stat rcinfo;
    FILE *rcstream;
    char *option, *nanorc_save = nanorc, *expanded;
    size_t lineno_save = lineno;

    option = ptr;
    if (*option == '"')
	option++;
    ptr = parse_argument(ptr);

    /* Can't get the specified file's full path cause it may screw up
	our cwd depending on the parent dirs' permissions, (see Savannah bug 25297) */

    /* Don't open directories, character files, or block files. */
    if (stat(option, &rcinfo) != -1) {
	if (S_ISDIR(rcinfo.st_mode) || S_ISCHR(rcinfo.st_mode) ||
		S_ISBLK(rcinfo.st_mode)) {
	    rcfile_error(S_ISDIR(rcinfo.st_mode) ?
		_("\"%s\" is a directory") :
		_("\"%s\" is a device file"), option);
	}
    }

    expanded = real_dir_from_tilde(option);

    /* Open the new syntax file. */
    if ((rcstream = fopen(expanded, "rb")) == NULL) {
	rcfile_error(_("Error reading %s: %s"), expanded,
		strerror(errno));
	return;
    }

    /* Use the name and line number position of the new syntax file
     * while parsing it, so we can know where any errors in it are. */
    nanorc = expanded;
    lineno = 0;

#ifdef DEBUG
    fprintf(stderr, "Parsing file \"%s\" (expanded from \"%s\")\n", expanded, option);
#endif

    parse_rcfile(rcstream
#ifdef ENABLE_COLOR
	, TRUE
#endif
	);

    /* We're done with the new syntax file.  Restore the original
     * filename and line number position. */
    nanorc = nanorc_save;
    lineno = lineno_save;

}

/* Return the short value corresponding to the color named in colorname,
 * and set bright to TRUE if that color is bright. */
short color_to_short(const char *colorname, bool *bright)
{
    short mcolor = -1;

    assert(colorname != NULL && bright != NULL);

    if (strncasecmp(colorname, "bright", 6) == 0) {
	*bright = TRUE;
	colorname += 6;
    }

    if (strcasecmp(colorname, "green") == 0)
	mcolor = COLOR_GREEN;
    else if (strcasecmp(colorname, "red") == 0)
	mcolor = COLOR_RED;
    else if (strcasecmp(colorname, "blue") == 0)
	mcolor = COLOR_BLUE;
    else if (strcasecmp(colorname, "white") == 0)
	mcolor = COLOR_WHITE;
    else if (strcasecmp(colorname, "yellow") == 0)
	mcolor = COLOR_YELLOW;
    else if (strcasecmp(colorname, "cyan") == 0)
	mcolor = COLOR_CYAN;
    else if (strcasecmp(colorname, "magenta") == 0)
	mcolor = COLOR_MAGENTA;
    else if (strcasecmp(colorname, "black") == 0)
	mcolor = COLOR_BLACK;
    else
	rcfile_error(N_("Color \"%s\" not understood.\n"
		"Valid colors are \"green\", \"red\", \"blue\",\n"
		"\"white\", \"yellow\", \"cyan\", \"magenta\" and\n"
		"\"black\", with the optional prefix \"bright\"\n"
		"for foreground colors."), colorname);

    return mcolor;
}

/* Parse the color string in the line at ptr, and add it to the current
 * file's associated colors.  If icase is TRUE, treat the color string
 * as case insensitive. */
void parse_colors(char *ptr, bool icase)
{
    short fg, bg;
    bool bright = FALSE, no_fgcolor = FALSE;
    char *fgstr;

    assert(ptr != NULL);

    if (syntaxes == NULL) {
	rcfile_error(
		N_("Cannot add a color command without a syntax command"));
	return;
    }

    if (*ptr == '\0') {
	rcfile_error(N_("Missing color name"));
	return;
    }

    fgstr = ptr;
    ptr = parse_next_word(ptr);

    if (strchr(fgstr, ',') != NULL) {
	char *bgcolorname;

	strtok(fgstr, ",");
	bgcolorname = strtok(NULL, ",");
	if (bgcolorname == NULL) {
	    /* If we have a background color without a foreground color,
	     * parse it properly. */
	    bgcolorname = fgstr + 1;
	    no_fgcolor = TRUE;
	}
	if (strncasecmp(bgcolorname, "bright", 6) == 0) {
	    rcfile_error(
		N_("Background color \"%s\" cannot be bright"),
		bgcolorname);
	    return;
	}
	bg = color_to_short(bgcolorname, &bright);
    } else
	bg = -1;

    if (!no_fgcolor) {
	fg = color_to_short(fgstr, &bright);

	/* Don't try to parse screwed-up foreground colors. */
	if (fg == -1)
	    return;
    } else
	fg = -1;

    if (*ptr == '\0') {
	rcfile_error(N_("Missing regex string"));
	return;
    }

    /* Now for the fun part.  Start adding regexes to individual strings
     * in the colorstrings array, woo! */
    while (ptr != NULL && *ptr != '\0') {
	colortype *newcolor;
	    /* The new color structure. */
	bool cancelled = FALSE;
	    /* The start expression was bad. */
	bool expectend = FALSE;
	    /* Do we expect an end= line? */

	if (strncasecmp(ptr, "start=", 6) == 0) {
	    ptr += 6;
	    expectend = TRUE;
	}

	if (*ptr != '"') {
	    rcfile_error(
		N_("Regex strings must begin and end with a \" character"));
	    ptr = parse_next_regex(ptr);
	    continue;
	}

	ptr++;

	fgstr = ptr;
	ptr = parse_next_regex(ptr);
	if (ptr == NULL)
	    break;

	newcolor = (colortype *)nmalloc(sizeof(colortype));

	/* Save the starting regex string if it's valid, and set up the
	 * color information. */
	if (nregcomp(fgstr, icase ? REG_ICASE : 0)) {
	    newcolor->fg = fg;
	    newcolor->bg = bg;
	    newcolor->bright = bright;
	    newcolor->icase = icase;

	    newcolor->start_regex = mallocstrcpy(NULL, fgstr);
	    newcolor->start = NULL;

	    newcolor->end_regex = NULL;
	    newcolor->end = NULL;

	    newcolor->next = NULL;

	    if (endcolor == NULL) {
		endsyntax->color = newcolor;
#ifdef DEBUG
		fprintf(stderr, "Starting a new colorstring for fg %hd, bg %hd\n", fg, bg);
#endif
	    } else {
#ifdef DEBUG
		fprintf(stderr, "Adding new entry for fg %hd, bg %hd\n", fg, bg);
#endif
		endcolor->next = newcolor;
	    }

	    endcolor = newcolor;
	} else {
	    free(newcolor);
	    cancelled = TRUE;
	}

	if (expectend) {
	    if (ptr == NULL || strncasecmp(ptr, "end=", 4) != 0) {
		rcfile_error(
			N_("\"start=\" requires a corresponding \"end=\""));
		return;
	    }
	    ptr += 4;
	    if (*ptr != '"') {
		rcfile_error(
			N_("Regex strings must begin and end with a \" character"));
		continue;
	    }

	    ptr++;

	    fgstr = ptr;
	    ptr = parse_next_regex(ptr);
	    if (ptr == NULL)
		break;

	    /* If the start regex was invalid, skip past the end regex to
	     * stay in sync. */
	    if (cancelled)
		continue;

	    /* Save the ending regex string if it's valid. */
	    newcolor->end_regex = (nregcomp(fgstr, icase ? REG_ICASE :
		0)) ? mallocstrcpy(NULL, fgstr) : NULL;

	    /* Lame way to skip another static counter */
            newcolor->id = endsyntax->nmultis;
            endsyntax->nmultis++;
	}
    }
}

/* Parse the headers (1st line) of the file which may influence the regex used. */
void parse_headers(char *ptr)
{
    char *regstr;

    assert(ptr != NULL);

    if (syntaxes == NULL) {
	rcfile_error(
		N_("Cannot add a header regex without a syntax command"));
	return;
    }

    if (*ptr == '\0') {
	rcfile_error(N_("Missing regex string"));
	return;
    }

    /* Now for the fun part.  Start adding regexes to individual strings
     * in the colorstrings array, woo! */
    while (ptr != NULL && *ptr != '\0') {
	exttype *newheader;
	    /* The new color structure. */

	if (*ptr != '"') {
	    rcfile_error(
		N_("Regex strings must begin and end with a \" character"));
	    ptr = parse_next_regex(ptr);
	    continue;
	}

	ptr++;

	regstr = ptr;
	ptr = parse_next_regex(ptr);
	if (ptr == NULL)
	    break;

	newheader = (exttype *)nmalloc(sizeof(exttype));

	/* Save the regex string if it's valid */
	if (nregcomp(regstr, 0)) {
	    newheader->ext_regex = mallocstrcpy(NULL, regstr);
	    newheader->ext = NULL;
	    newheader->next = NULL;

#ifdef DEBUG
	     fprintf(stderr, "Starting a new header entry: %s\n", newheader->ext_regex);
#endif

	    if (endheader == NULL) {
		endsyntax->headers = newheader;
	    } else {
		endheader->next = newheader;
	    }

	    endheader = newheader;
	} else
	    free(newheader);

    }
}
#endif /* ENABLE_COLOR */

/* Check whether the user has unmapped every shortcut for a
sequence we consider 'vital', like the exit function */
static void check_vitals_mapped(void)
{
    subnfunc *f;
    int v;
#define VITALS 5
    short vitals[VITALS] = { DO_EXIT, DO_EXIT, CANCEL_MSG, CANCEL_MSG, CANCEL_MSG };
    int inmenus[VITALS] = { MMAIN, MHELP, MWHEREIS, MREPLACE, MGOTOLINE };

    for  (v = 0; v < VITALS; v++) {
       for (f = allfuncs; f != NULL; f = f->next) {
           if (f->scfunc == vitals[v] && f->menus & inmenus[v]) {
               const sc *s = first_sc_for(inmenus[v], f->scfunc);
               if (!s) {
                   rcfile_error(N_("Fatal error: no keys mapped for function \"%s\""),
                       f->desc);
                   fprintf(stderr, N_("Exiting.  Please use nano with the -I option if needed to adjust your nanorc settings\n"));
                   exit(1);
               }
           break;
           }
       }
    }
}

/* Parse the rcfile, once it has been opened successfully at rcstream,
 * and close it afterwards.  If syntax_only is TRUE, only allow the file
 * to contain color syntax commands: syntax, color, and icolor. */
void parse_rcfile(FILE *rcstream
#ifdef ENABLE_COLOR
	, bool syntax_only
#endif
	)
{
    char *buf = NULL;
    ssize_t len;
    size_t n = 0;

    while ((len = getline(&buf, &n, rcstream)) > 0) {
	char *ptr, *keyword, *option;
	int set = 0;
	size_t i;

	/* Ignore the newline. */
	if (buf[len - 1] == '\n')
	    buf[len - 1] = '\0';

	lineno++;
	ptr = buf;
	while (isblank(*ptr))
	    ptr++;

	/* If we have a blank line or a comment, skip to the next
	 * line. */
	if (*ptr == '\0' || *ptr == '#')
	    continue;

	/* Otherwise, skip to the next space. */
	keyword = ptr;
	ptr = parse_next_word(ptr);

	/* Try to parse the keyword. */
	if (strcasecmp(keyword, "set") == 0) {
#ifdef ENABLE_COLOR
	    if (syntax_only)
		rcfile_error(
			N_("Command \"%s\" not allowed in included file"),
			keyword);
	    else
#endif
		set = 1;
	} else if (strcasecmp(keyword, "unset") == 0) {
#ifdef ENABLE_COLOR
	    if (syntax_only)
		rcfile_error(
			N_("Command \"%s\" not allowed in included file"),
			keyword);
	    else
#endif
		set = -1;
	}
#ifdef ENABLE_COLOR
	else if (strcasecmp(keyword, "include") == 0) {
	    if (syntax_only)
		rcfile_error(
			N_("Command \"%s\" not allowed in included file"),
			keyword);
	    else
		parse_include(ptr);
	} else if (strcasecmp(keyword, "syntax") == 0) {
	    if (endsyntax != NULL && endcolor == NULL)
		rcfile_error(N_("Syntax \"%s\" has no color commands"),
			endsyntax->desc);
	    parse_syntax(ptr);
	} else if (strcasecmp(keyword, "header") == 0)
	    parse_headers(ptr);
	else if (strcasecmp(keyword, "color") == 0)
	    parse_colors(ptr, FALSE);
	else if (strcasecmp(keyword, "icolor") == 0)
	    parse_colors(ptr, TRUE);
	else if (strcasecmp(keyword, "bind") == 0)
	    parse_keybinding(ptr);
	else if (strcasecmp(keyword, "unbind") == 0)
	    parse_unbinding(ptr);
#endif /* ENABLE_COLOR */
	else
	    rcfile_error(N_("Command \"%s\" not understood"), keyword);

	if (set == 0)
	    continue;

	if (*ptr == '\0') {
	    rcfile_error(N_("Missing flag"));
	    continue;
	}

	option = ptr;
	ptr = parse_next_word(ptr);

	for (i = 0; rcopts[i].name != NULL; i++) {
	    if (strcasecmp(option, rcopts[i].name) == 0) {
#ifdef DEBUG
		fprintf(stderr, "parse_rcfile(): name = \"%s\"\n", rcopts[i].name);
#endif
		if (set == 1) {
		    if (rcopts[i].flag != 0)
			/* This option has a flag, so it doesn't take an
			 * argument. */
			SET(rcopts[i].flag);
		    else {
			/* This option doesn't have a flag, so it takes
			 * an argument. */
			if (*ptr == '\0') {
			    rcfile_error(
				N_("Option \"%s\" requires an argument"),
				rcopts[i].name);
			    break;
			}
			option = ptr;
			if (*option == '"')
			    option++;
			ptr = parse_argument(ptr);

			option = mallocstrcpy(NULL, option);
#ifdef DEBUG
			fprintf(stderr, "option = \"%s\"\n", option);
#endif

			/* Make sure option is a valid multibyte
			 * string. */
			if (!is_valid_mbstring(option)) {
			    rcfile_error(
				N_("Option is not a valid multibyte string"));
			    break;
			}

#ifndef DISABLE_OPERATINGDIR
			if (strcasecmp(rcopts[i].name, "operatingdir") == 0)
			    operating_dir = option;
			else
#endif
#ifndef DISABLE_WRAPJUSTIFY
			if (strcasecmp(rcopts[i].name, "fill") == 0) {
			    if (!parse_num(option, &wrap_at)) {
				rcfile_error(
					N_("Requested fill size \"%s\" is invalid"),
					option);
				wrap_at = -CHARS_FROM_EOL;
			    } else
				free(option);
			} else
#endif
#ifndef NANO_TINY
			if (strcasecmp(rcopts[i].name,
				"matchbrackets") == 0) {
			    matchbrackets = option;
			    if (has_blank_mbchars(matchbrackets)) {
				rcfile_error(
					N_("Non-blank characters required"));
				free(matchbrackets);
				matchbrackets = NULL;
			    }
			} else if (strcasecmp(rcopts[i].name,
				"whitespace") == 0) {
			    whitespace = option;
			    if (mbstrlen(whitespace) != 2 ||
				strlenpt(whitespace) != 2) {
				rcfile_error(
					N_("Two single-column characters required"));
				free(whitespace);
				whitespace = NULL;
			    } else {
				whitespace_len[0] =
					parse_mbchar(whitespace, NULL,
					NULL);
				whitespace_len[1] =
					parse_mbchar(whitespace +
					whitespace_len[0], NULL, NULL);
			    }
			} else
#endif
#ifndef DISABLE_JUSTIFY
			if (strcasecmp(rcopts[i].name, "punct") == 0) {
			    punct = option;
			    if (has_blank_mbchars(punct)) {
				rcfile_error(
					N_("Non-blank characters required"));
				free(punct);
				punct = NULL;
			    }
			} else if (strcasecmp(rcopts[i].name,
				"brackets") == 0) {
			    brackets = option;
			    if (has_blank_mbchars(brackets)) {
				rcfile_error(
					N_("Non-blank characters required"));
				free(brackets);
				brackets = NULL;
			    }
			} else if (strcasecmp(rcopts[i].name,
				"quotestr") == 0)
			    quotestr = option;
			else
#endif
#ifndef NANO_TINY
			if (strcasecmp(rcopts[i].name,
				"backupdir") == 0)
			    backup_dir = option;
			else
#endif
#ifndef DISABLE_SPELLER
			if (strcasecmp(rcopts[i].name, "speller") == 0)
			    alt_speller = option;
			else
#endif
			if (strcasecmp(rcopts[i].name,
				"tabsize") == 0) {
			    if (!parse_num(option, &tabsize) ||
				tabsize <= 0) {
				rcfile_error(
					N_("Requested tab size \"%s\" is invalid"),
					option);
				tabsize = -1;
			    } else
				free(option);
			} else
			    assert(FALSE);
		    }
#ifdef DEBUG
		    fprintf(stderr, "flag = %ld\n", rcopts[i].flag);
#endif
		} else if (rcopts[i].flag != 0)
		    UNSET(rcopts[i].flag);
		else
		    rcfile_error(N_("Cannot unset flag \"%s\""),
			rcopts[i].name);
		/* Looks like we still need this specific hack for undo */
		if (strcasecmp(rcopts[i].name, "undo") == 0)
		    shortcut_init(0);
		break;
	    }
	}
	if (rcopts[i].name == NULL)
	    rcfile_error(N_("Unknown flag \"%s\""), option);
    }

#ifdef ENABLE_COLOR
    if (endsyntax != NULL && endcolor == NULL)
	rcfile_error(N_("Syntax \"%s\" has no color commands"),
		endsyntax->desc);
#endif

    free(buf);
    fclose(rcstream);
    lineno = 0;

    check_vitals_mapped();
    return;
}

/* The main rcfile function.  It tries to open the system-wide rcfile,
 * followed by the current user's rcfile. */
void do_rcfile(void)
{
    struct stat rcinfo;
    FILE *rcstream;

    nanorc = mallocstrcpy(nanorc, SYSCONFDIR "/nanorc");

    /* Don't open directories, character files, or block files. */
    if (stat(nanorc, &rcinfo) != -1) {
	if (S_ISDIR(rcinfo.st_mode) || S_ISCHR(rcinfo.st_mode) ||
		S_ISBLK(rcinfo.st_mode))
	    rcfile_error(S_ISDIR(rcinfo.st_mode) ?
		_("\"%s\" is a directory") :
		_("\"%s\" is a device file"), nanorc);
    }

#ifdef DEBUG
    fprintf(stderr, "Parsing file \"%s\"\n", nanorc);
#endif

    /* Try to open the system-wide nanorc. */
    rcstream = fopen(nanorc, "rb");
    if (rcstream != NULL)
	parse_rcfile(rcstream
#ifdef ENABLE_COLOR
		, FALSE
#endif
		);

#ifdef DISABLE_ROOTWRAPPING
    /* We've already read SYSCONFDIR/nanorc, if it's there.  If we're
     * root, and --disable-wrapping-as-root is used, turn wrapping off
     * now. */
    if (geteuid() == NANO_ROOT_UID)
	SET(NO_WRAP);
#endif

    get_homedir();

    if (homedir == NULL)
	rcfile_error(N_("I can't find my home directory!  Wah!"));
    else {
#ifndef RCFILE_NAME
#define RCFILE_NAME ".nanorc"
#endif
	nanorc = charealloc(nanorc, strlen(homedir) + strlen(RCFILE_NAME) + 2);
	sprintf(nanorc, "%s/%s", homedir, RCFILE_NAME);

	/* Don't open directories, character files, or block files. */
	if (stat(nanorc, &rcinfo) != -1) {
	    if (S_ISDIR(rcinfo.st_mode) || S_ISCHR(rcinfo.st_mode) ||
		S_ISBLK(rcinfo.st_mode))
		rcfile_error(S_ISDIR(rcinfo.st_mode) ?
			_("\"%s\" is a directory") :
			_("\"%s\" is a device file"), nanorc);
	}

	/* Try to open the current user's nanorc. */
	rcstream = fopen(nanorc, "rb");
	if (rcstream == NULL) {
	    /* Don't complain about the file's not existing. */
	    if (errno != ENOENT)
		rcfile_error(N_("Error reading %s: %s"), nanorc,
			strerror(errno));
	} else
	    parse_rcfile(rcstream
#ifdef ENABLE_COLOR
		, FALSE
#endif
		);
    }

    free(nanorc);
    nanorc = NULL;

    if (errors && !ISSET(QUIET)) {
	errors = FALSE;
	fprintf(stderr,
		_("\nPress Enter to continue starting nano.\n"));
	while (getchar() != '\n')
	    ;
    }

#ifdef ENABLE_COLOR
    set_colorpairs();
#endif
}

#endif /* ENABLE_NANORC */
