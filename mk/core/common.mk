#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#

ifndef __core_common_mk
__core_common_mk := 1

## Empty string.
\empty :=

## Single space.
\space := $(subst ,, )
\s     := $(\space)

##
# This variable named by a single space expands to empty string.
# Perhaps, it is not useful "as is", but
#
$(\space) :=

define \n


endef
\n := $(\n)

\t := $(subst ,,	)

         \n          := $(\n)
$(\space)\n          := $(\n)
$(\space)\n$(\space) := $(\n)
         \n$(\space) := $(\n)

\h := \#

\comma := ,

\colon := :

\brace_open = (
\brace_close = )

\equal := =

# Identity function.
id = $1

true  := T
false :=

#
# Function:  not
# Arguments: 1: A boolean value
# Returns:   Returns the opposite of the arg. (true -> false, false -> true)
#
not = $(if $1,$(false),$(true))

make_bool = \
  $(if $(strip $1),$(true),$(false))
make_bool_unstripped = \
  $(if $1,$(true),$(false))

endif # __core_common_mk
