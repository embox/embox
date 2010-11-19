#
# Well, it is not quite a sandbox, as one might think. It's just a kind of
# wrapper around some code which filters all newly defined variables.
#
#   Date: Nov 19, 2010
# Author: Eldar Abusalimov
#

ifndef sandbox # do not hold special variable for inclusion guard

#
# Function: sandbox
#
# Interprets the argument as 'make' code and filters all variables defined by
# that code. Please note that only newly defined variables are returned,
# changes of existing variables are not tracked.
#
# Params:
#  1. 'make' code to be interpreted.
#
# Returns: list of variables defined by the interpreted code.
#
sandbox = $(eval $(value __sandbox_mk))$(__sandbox_variables)

define __sandbox_mk
  __sandbox_variables := __sandbox_variables $(.VARIABLES)
  $(eval $1)
  __sandbox_variables := $(filter-out $(__sandbox_variables),$(.VARIABLES))
endef

endif # sandbox
