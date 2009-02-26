/*
 * vt.h
 *
 * Provides VT/ANSI terminal control sequence constants.
 *
 * See: http://en.wikipedia.org/wiki/ANSI_escape_code
 * See: http://ascii-table.com/ansi-escape-sequences-vt-100.php
 * See: http://www.termsys.demon.co.uk/vtansi.htm
 *
 * Author: Eldar Abusalimov
 */

#ifndef VT_H_
#define VT_H_

#define MAX_PARAMS	16

typedef struct _INT_ARRAY {
	int data[MAX_PARAMS];
	int length;
} VT_PARAMS;

typedef enum {
	VT_ACTION_CLEAR = 1,
	VT_ACTION_COLLECT = 2,
	VT_ACTION_CSI_DISPATCH = 3,
	VT_ACTION_ESC_DISPATCH = 4,
	VT_ACTION_EXECUTE = 5,
	VT_ACTION_HOOK = 6,
	VT_ACTION_IGNORE = 7,
	VT_ACTION_OSC_END = 8,
	VT_ACTION_OSC_PUT = 9,
	VT_ACTION_OSC_START = 10,
	VT_ACTION_PARAM = 11,
	VT_ACTION_PRINT = 12,
	VT_ACTION_PUT = 13,
	VT_ACTION_UNHOOK = 14,
} VT_ACTION;

/*
 * Codes used as final bytes of ANSI control sequences
 * (VT_ACTION_CSI_DISPATCH)
 */
typedef enum {

	/* Cursor Up */
	VT_CODE_CS_CUU = 'A',

	/* Cursor Down */
	VT_CODE_CS_CUD = 'B',

	/* Cursor Forward */
	VT_CODE_CS_CUF = 'C',

	/* Cursor Backward */
	VT_CODE_CS_CUB = 'D',

	/* Cursor Position */
	VT_CODE_CS_CUP = 'H',

	/*
	 * Clear part of the screen.
	 * If attr is zero (or missing), clear from cursor to end of screen.
	 * If attr is one, clear from cursor to beginning of the screen.
	 * If attr is two, clear entire screen
	 */
	VT_CODE_CS_ED = 'J',

	/*
	 * Erases part of the line.
	 * If attr is zero (or missing), clear from cursor to the end of the line.
	 * If attr is one, clear from cursor to beginning of the line.
	 * If attr is two, clear entire line.
	 * Cursor position does not change
	 */
	VT_CODE_CS_EL = 'K',

	/* Saves the cursor position. */
	VT_CODE_CS_SCP = 's',

	/* Restores the cursor position. */
	VT_CODE_CS_RCP = 'u',

} VT_CODE_CS;

/*
 * Codes used as final bytes of escape sequences
 * (VT_ACTION_ESC_DISPATCH)
 */
typedef enum {

	/* Saves the cursor position & attributes. */
	VT_CODE_ESC_SCA = '7',

	/* Restores the cursor position & attributes. */
	VT_CODE_ESC_RCA = '8',

} VT_CODE_ESC;

// ANSI Escape
#define ESC		'\e'
// ANSI Control Sequence Introducer
#define CSI		'['

// Select Graphic Rendition
#define ANSI_CODE_SGR				'm'

#define ANSI_CODE_SET_MODE			'h'
#define ANSI_CODE_RESET_MODE		'l'

#define ANSI_PARAM_MODE_LINE_WRAP	'7'

#define ANSI_CODE_ERASE_LINE		'K'
#define ANSI_CODE_ERASE_SCREEN		'J'

#define ANSI_PARAM_ERASE_DOWN_RIGHT	'0'
#define ANSI_PARAM_ERASE_UP_LEFT	'1'
#define ANSI_PARAM_ERASE_ENTIRE		'2'

#define SGR_COLOR_BLACK		0
#define SGR_COLOR_RED		1

#define PRV_PARAM_HOME		'2'
#define PRV_PARAM_END		'5'
#define PRV_PARAM_DELETE	'3'

//   # Erasing
//
//    DEF	eeol    escb K  ; # Erase (to) End Of Line
//    DEF	esol    escb 1K ; # Erase (to) Start Of Line
//    DEF	el      escb 2K ; # Erase (current) Line
//    DEF	ed      escb J  ; # Erase Down (to bottom)
//    DEF	eu      escb 1J ; # Erase Up (to top)
//    DEF	es      escb 2J ; # Erase Screen
//
//    # Scrolling
//
//    DEF	sd      esc D    ; # Scroll Down
//    DEF	su      esc M    ; # Scroll Up
//
//    # Cursor Handling
//
//    DEF	ch      escb H  ; # Cursor Home
//    DEF	sc      escb s  ; # Save Cursor
//    DEF	rc      escb u  ; # Restore Cursor (Unsave)
//    DEF	sca     esc  7  ; # Save Cursor + Attributes
//    DEF	rca     esc  8  ; # Restore Cursor + Attributes
//
//    # Tabbing
//
//    DEF	st      esc  H  ; # Set Tab (@ current position)
//    DEF	ct      escb g  ; # Clear Tab (@ current position)
//    DEF	cat     escb 3g ; # Clear All Tabs
//
//    # Device Introspection
//
//    DEF	qdc     escb c  ; # Query Device Code
//    DEF	qds     escb 5n ; # Query Device Status
//    DEF	qcp     escb 6n ; # Query Cursor Position
//    DEF	rd      esc  c  ; # Reset Device
//
//    # Linewrap on/off
//
//    DEF	elw     escb 7h ; # Enable Line Wrap
//    DEF	dlw     escb 7l ; # Disable Line Wrap
//
//    # Graphics Mode (aka use alternate font on/off)
//
//    DEF eg      esc F   ; # Enter Graphics Mode
//    DEF lg      esc G   ; # Exit Graphics Mode
//
//    ##
//    # ### ### ### ######### ######### #########
//
//    # ### ### ### ######### ######### #########
//    ## Complex, parameterized codes
//
//    # Select Character Set
//    # Choose which char set is used for default and
//    # alternate font. This does not change whether
//    # default or alternate font are used
//
//    DEFC scs0 {tag} {esc  ($tag}  ; # Set default character set
//    DEFC scs1 {tag} {esc  )$tag}  ; # Set alternate character set
//
//    # tags in A : United Kingdom Set
//    #         B : ASCII Set
//    #         0 : Special Graphics
//    #         1 : Alternate Character ROM Standard Character Set
//    #         2 : Alternate Character ROM Special Graphics
//
//    # Set Display Attributes
//
//    DEFC sda {args} {escb [join $args ";"]m}
//
//    # Force Cursor Position (aka Go To)
//
//    DEFC fcp {r c}  {escb ${r}\;${c}f}
//
//    # Cursor Up, Down, Forward, Backward
//
//    DEFC cu {{n 1}} {escb [expr {$n == 1 ? "A" : "${n}A"}]}
//    DEFC cd {{n 1}} {escb [expr {$n == 1 ? "B" : "${n}B"}]}
//    DEFC cf {{n 1}} {escb [expr {$n == 1 ? "C" : "${n}C"}]}
//    DEFC cb {{n 1}} {escb [expr {$n == 1 ? "D" : "${n}D"}]}
//
//    # Scroll Screen (entire display, or between rows start end, inclusive).
//
//    DEFC ss {args} {
//	if {[llength $args] == 0} {return [escb r]}
//	if {[llength $args] == 2} {foreach {s e} $args break ; return [escb ${s};${e}r]}
//	return -code error "wrong\#args"
//    }
//
//    # Set Key Definition
//
//    DEFC skd {code str} {escb "${code};\"${str}\"p"}
//
//    # Terminal title
//
//    DEFC title {str} {esc "\]0;${str}\007"}
//
//    # Switch to and from character/box graphics.
//
//    DEFC gron  {} {return \016}
//    DEFC groff {} {return \017}
//
//    # Character graphics, box symbols
//    # - 4 corners, 4 t-junctions,
//    #   one 4-way junction, 2 lines
//
//    DEFC tlc   {} {return [gron]l[groff]} ; # Top    Left  Corner
//    DEFC trc   {} {return [gron]k[groff]} ; # Top    Right Corner
//    DEFC brc   {} {return [gron]j[groff]} ; # Bottom Right Corner
//    DEFC blc   {} {return [gron]m[groff]} ; # Bottom Left  Corner
//
//    DEFC ltj   {} {return [gron]t[groff]} ; # Left   T Junction
//    DEFC ttj   {} {return [gron]w[groff]} ; # Top    T Junction
//    DEFC rtj   {} {return [gron]u[groff]} ; # Right  T Junction
//    DEFC btj   {} {return [gron]v[groff]} ; # Bottom T Junction
//
//    DEFC fwj   {} {return [gron]n[groff]} ; # Four-Way Junction
//
//    DEFC hl    {} {return [gron]q[groff]} ; # Horizontal Line
//    DEFC vl    {} {return [gron]x[groff]} ; # Vertical   Line
//
//    # Optimize character graphics. The generator commands above create
//    # way to many superfluous commands shifting into and out of the
//    # graphics mode. The command below removes all shifts which are
//    # not needed. To this end it also knows which characters will look
//    # the same in both modes, to handle strings created outside this
//    # package.
//
//    DEFC groptim {string} {
//	variable grforw
//	variable grback
//	while {![string equal $string [set new [string map \
//		"\017\016 {} \016\017 {}" [string map \
//		$grback [string map \
//		$grforw $string]]]]]} {
//	    set string $new
//	}
//	return $string
//    }
//
//    ##
//    # ### ### ### ######### ######### #########
//
//    # ### ### ### ######### ######### #########
//    ## Higher level operations
//
//    # Clear screen <=> CursorHome + EraseDown
//    # Init (Fonts): Default ASCII, Alternate Graphics
//    # Show a block of text at a specific location.
//
//    DEFC clear {} {return [ch][ed]}
//    DEFC init  {} {return [scs0 B][scs1 0]}
//
//    DEFC showat {r c text} {
//	if {![string length $text]} {return {}}
//	return [fcp $r $c][sca][join \
//		[split $text \n] \
//		[rca][cd][sca]][rca][cd]
//    }
//
//    ##
//    # ### ### ### ######### ######### #########
//
//    # ### ### ### ######### ######### #########
//    ## Attribute control (single attributes)
//
//    foreach a [::term::ansi::code::attr::names] {
//	DEF sda_$a escb "[::term::ansi::code::attr::$a]m"
//    }
//
//    ##
//    # ### ### ### ######### ######### #########

#endif /* VT_H_ */
