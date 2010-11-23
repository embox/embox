#
# Traverser entity sandbox.
#
#   Date: Nov 15, 2010
# Author: Eldar Abusalimov
#

ifndef __embuild_traverse_entity_mk
__embuild_traverse_entity_mk := 1

include gmsl.mk # TODO for 'tr' function. -- Eldar

entity = $1-$2

entity_print = $(subst -, ,$1)

entity_type = $(call __entity_type,$(__entity_split))
entity_name = $(call __entity_name,$(__entity_split))

entity_types = $(foreach e,$1,$(call entity_type,$e))
entity_names = $(foreach e,$1,$(call entity_name,$e))

# Params:
#  1. entity name to check
#  2. all entities to filter from
# Returns: list of entity types for the specified name (duplicates are allowed)
entity_types_for_name = $(strip \
  $(foreach e,$2, \
    $(if $(filter $1,$(call entity_name,$e)),$(call entity_type,$e)) \
   ) \
)

__entity_type = $(word 1,$1)
__entity_name = $(word 3,$1)

__entity_split = $(subst -, - ,$1)

entity_check = \
  $(if $(filter 1,$(words $1)),$(call __entity_check,$(__entity_split)))
__entity_check = \
  $(if $(and $(filter 3,$(words $1)), \
             $(call entity_check_type,$(__entity_type)), \
             $(call entity_check_name,$(__entity_name))),$(subst $(\space),,$1))

entity_valid_types := api module library package

entity_check_type = \
  $(and $(filter 1,$(words $1)), \
        $(filter $(entity_valid_types),$1))

define spec(entity_check_type)
  $~scene = It should accept only these types: api module library package
    foreach_given_arg = $(entity_valid_types)
    result_should_be  = $1
  $~scene = Any other types should be rejected
    foreach_given_arg = foo bar baz sdf blah
    result_should_be  = $(empty)
endef

entity_check_name = \
  $(and $(filter 1,$(words $1)), \
        $(if $(call tr,$([A-Z]) $([a-z]) $([0-9]) _,,$1),,$1))

endif # __embuild_traverse_entity_mk


