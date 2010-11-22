#
# Traverser entity sandbox.
#
#   Date: Nov 15, 2010
# Author: Eldar Abusalimov
#

include gmsl.mk # TODO for 'tr' function. -- Eldar

entity = \
  $(if $(and $(call entity_type_check,$1),$(call entity_name_check,$2)),$1-$2)

entity_type = $(call __entity_type,$(subst -, ,$1))
__entity_type = $()

entity_check = \
  $(if $(filter 1,$(words $1)),$(call __entity_check,$(subst -, - ,$1)))
__entity_check = \
  $(if $(and $(filter 3,$(words $1)), \
             $(call entity_check_type,$(word 1,$1)), \
             $(call entity_check_name,$(word 2,$1))),$(subst $(\space),,$1))

entity_types := api module library package

entity_check_type = \
  $(and $(filter 1,$(words $1)), \
        $(filter $(entity_types),$1))

entity_check_name = \
  $(and $(filter 1,$(words $1)), \
        $(if $(call tr,$([A-Z]) $([a-z]) $([0-9]) _,,$1),,$1))

ifdef __EMBUILD_TRAVERSE_ENTITY

# Only essential scripts are included here to avoid possible collisions
# with variables of sandboxed entity.
include util/envdef.mk
include util/sandbox.mk

$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_ENTITY_NAME \
  __EMBUILD_TRAVERSE_ENTITY_TYPE \
  __EMBUILD_TRAVERSE_ENTITY_FILE \
  __EMBUILD_TRAVERSE_ENTITY      \
,Traverser entity sandbox needs these variables to be defined)

# Gogogo!
__embuild_traverse_entity_variables := $(call sandbox, \
  $(value __EMBUILD_TRAVERSE_ENTITY) \
)

include util/common.mk

prereq = $(__EMBUILD_TRAVERSE_ENTITY_FILE)
target = $(OBJ_DIR)/$(__EMBUILD_TRAVERSE_ENTITY_NAME).mk

variable_dump = \
  define $$_$1\n$(subst $(\n),\n,$(subst \,\\,$(value $1)))\nendef
variable_dump_all = \
  $(foreach var,$1,$(call variable_dump,$(var))\n)

$(target) : $(prereq)
	@mkdir -p $(@D)
	@$(PRINTF) '# Auto-generated EMBuild entity. Do not edit.\n' > $@
	$(PRINTF) '$(call variable_dump_all,$(__embuild_traverse_emfile_variables))' >> $@

endif # __EMBUILD_TRAVERSE_ENTITY
