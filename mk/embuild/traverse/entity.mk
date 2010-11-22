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

entity_type = $(call __entity_type,$(__entity_split))
entity_name = $(call __entity_name,$(__entity_split))

__entity_type = $(word 1,$1)
__entity_name = $(word 3,$1)

__entity_split = $(subst -. - ,$1)

entity_check = \
  $(if $(filter 1,$(words $1)),$(call __entity_check,$(__entity_split)))
__entity_check = \
  $(if $(and $(filter 3,$(words $1)), \
             $(call entity_check_type,$(__entity_type)), \
             $(call entity_check_name,$(__entity_name))),$(subst $(\space),,$1))

entity_types := api module library package

entity_check_type = \
  $(and $(filter 1,$(words $1)), \
        $(filter $(entity_types),$1))

entity_check_name = \
  $(and $(filter 1,$(words $1)), \
        $(if $(call tr,$([A-Z]) $([a-z]) $([0-9]) _,,$1),,$1))

endif # __embuild_traverse_entity_mk


