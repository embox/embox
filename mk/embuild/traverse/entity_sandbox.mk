#
# Traverser entity sandbox.
#
#   Date: Nov 15, 2010
# Author: Eldar Abusalimov
#

# Only essential scripts are included here to avoid possible collisions
# with variables of sandboxed entity.
include util/envdef.mk

$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_ENTITY_VALUE \
  __EMBUILD_TRAVERSE_ENTITY_FILE  \
  __EMBUILD_TRAVERSE_ENTITY       \
,Traverser entity sandbox needs these variables to be defined)

__entity_sandbox_variables_before :=
__entity_sandbox_variables_before := $(.VARIABLES)

# Gogogo!
${eval __EMBUILD_TRAVERSE_ENTITY_VALUE}

__entity_sandbox_variables_after  := $(.VARIABLES)

# Process cathed variables.
include embuild/traverse/entity.mk

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
