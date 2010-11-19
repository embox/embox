#
# Utility for checking whether some variables are defined or not.
# It is primarily useful applied to environmental checks.
#
#   Date: Nov 18, 2010
# Author: Eldar Abusalimov
#

ifndef __util_envdef_mk
__util_envdef_mk := 1

#
# Function: envdef_assert_defined
#
# Checks whether each of the specified variables is defined and fails if not.
#
# Params:
#  1. The names of variables to check
#
# Returns: nothing
#
envdef_assert_defined = \
  $(and $(foreach var,$(call envdef_filter_undefined,$1),x \
      $(warning Undefined variable: [$(var)])),$(error $2))

#
# Function: envdef_filter_undefined
#
# Filters undefined variables from the specified list.
#
# Params:
#  1. The names of variables to check
#
# Returns: list of undefined variables (if any).
#
envdef_filter_undefined = \
  $(strip $(foreach var,$1,$(if $(filter undefined,$(flavor $(var))),$(var))))

endif # __util_envdef_mk
