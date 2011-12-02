#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#

ifndef __core_common_mk
__core_common_mk := 1

## Empty string.
\0 :=

## Single space.
\s := $(subst ,, )

## Tab.
\t := $(subst ,,	)

## Newline.
define \n


endef
\n := $(\n)

## Hash.
\h := \#

## Single backslash: '\'
\\ := \\# Ignored comment.

## Parens (smile!).
[:=(
]:=)

## Punctuation.
\comma   := ,
\period  := .
\eq_sign := =

## Single dollar ('$') which expands into itself.
$$ := $$

##
# This variable is named by a single space and it expands to an empty string.
# Perhaps, it is not useful "as is", but it is widely used to suppress a space
# which is emitted when using backslash-newline in a function definition.
# Typical usage is:
#   foo = bar$ \
#         baz
# In this example the value of foo will be 'barbaz' without any whitespaces
# inside.
$(\s) :=

ascii_table = \
       SOH STX ETX EOT ENQ ACK BEL  BS TAB  LF  VT  FF  CR  SO  SI   \
   DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US   \
    SP   !   "  \#   $   %   &   '   (   )   *   +   ,   —   .   /   \
     0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?   \
     @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   \
     P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _   \
     `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o   \
     p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
ascii_table := $(strip $(value ascii_table))

# Params:
#   1. Char code.
ascii_char = \
	$(word $1,$(ascii_table))

## Identity function.
id = $1
builtin_func-id = $(builtin_to_function_inline)

true  := 1
false :=

##
# Builtin function: not
# Params:
#   1. A string to negate.
# Return:
#   '1' for empty argument, or empty value otherwise.
not = $(if $1,,1)
builtin_func-not = $(builtin_to_function_inline)

make_bool = \
  $(if $(strip $1),$(true),$(false))
make_bool_unstripped = \
  $(if $1,$(true),$(false))

endif # __core_common_mk
