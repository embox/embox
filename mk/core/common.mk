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

## Identity function.
id = $1

true  := 1
false :=

##
# Builtin function: not
# Params:
#   1. A string to negate.
# Return:
#   '1' for empty argument, or empty value otherwise.
not = $(if $1,,1)
builtin_func_not = $(builtin_to_function_inline)

make_bool = \
  $(if $(strip $1),$(true),$(false))
make_bool_unstripped = \
  $(if $1,$(true),$(false))

endif # __core_common_mk
