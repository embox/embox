#
# Traverser entity sandbox.
#
#   Date: Nov 15, 2010
# Author: Eldar Abusalimov
#

__embuild_traverse_entity_needs := \
  __EMBUILD_TRAVERSE_ENTITY_NAME \
  __EMBUILD_TRAVERSE_ENTITY_TYPE \
  __EMBUILD_TRAVERSE_ENTITY_FILE \
  __EMBUILD_TRAVERSE_ENTITY

__embuild_traverse_entity_needs_undefined := $(strip \
  $(foreach var,$(__embuild_traverse_entity_needs), \
    $(if $(filter undefined,$(flavor $(var))),$(var)) \
  ) \
)

ifneq ($(__embuild_traverse_entity_needs_undefined),)
define error_msg
Traverser entity sandbox needs the following variables to be defined:
  $(__embuild_traverse_entity_needs_undefined)
endef
$(error $(error_msg))
endif

$(foreach var,$(__embuild_traverse_entity_needs),$(warning $(var) = $($(var))))
