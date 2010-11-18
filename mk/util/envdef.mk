#
# Utility for checking whether some variables are defined or not.
# It is primarily useful applied to environmental checks.
#
#   Date: Nov 18, 2010
# Author: Eldar Abusalimov
#

envdef_assert_defined = \
  $(and $(foreach var,$(call envdef_filter_undefined,$1),x \
      $(warning Undefined variable: [$(var)])),$(error $2))

envdef_filter_undefined = \
  $(strip $(foreach var,$1,$(if $(filter undefined,$(flavor $(var))),$(var))))

