#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#

ifndef __util_common_mk
__util_common_mk := 1

\empty :=

\space := $(subst ,, )

$(\space) :=

define \n


endef
\n := $(\n)

\t := $(subst ,,	)

         \n          := $(\n)
$(\space)\n          := $(\n)
$(\space)\n$(\space) := $(\n)
         \n$(\space) := $(\n)

\comma := ,

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

endif # __util_common_mk
