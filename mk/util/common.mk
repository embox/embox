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

# Alias for \n (backward compatibility)
N := $(\n)

endif # __util_common_mk
