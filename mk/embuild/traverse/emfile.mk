#
# Traverser sandbox for em-files.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

include util/common.mk
include embuild/traverse/entity.mk

whitespace_escape = \
  $(call prefix_subst,$(subst $$,$$$$,$1), ,_$$_,$(subst $$,$$$$,$2))
whitespace_unescape = \
  $(subst $$$$,$$,$(subst _$$_, ,$2))

# TODO move somewhere. -- Eldar
prefix_subst = \
  $(if $(findstring $1$2,$4),$(call $0,$1$3,$(subst $1$2,$1$3,$4)),$4)

#$(foreach type,$(embuild_entity_types),$ \
  $(foreach escaped,$(call whitespace_escape,$(type),$(embuild_emfile_variables)), \
    $(strip $(call whitespace_unescape,$(type),$(escaped))) \
  ) \
)

embuild_emfile_entity_variables_escaped = \
  $(foreach type,$(embuild_entity_types), \
    $(filter $(type)%,$(call whitespace_escape,$(type),$(emfile_variables))) \
  )

embuild_emfile_entity_unescape_type = $(word 1,$(call whitespace_unescape,$1))
embuild_emfile_entity_unescape_name = $(word 2,$(call whitespace_unescape,$1))

__embuild_traverse_emfile_entity_variables = \
  $(filter $(embuild_entity_types:%=%-%),$(emfile_variables))

# Can't detect type of the entity (bogus type/name).
__embuild_traverse_emfile_bogus_variables = \
  $(filter-out __embuild_traverse_emfile_entity_variables, \
           $(__embuild_traverse_emfile_variables))

__embuild_traverse_emfile_entity_targets := \
  $(__embuild_traverse_emfile_entity_variables:%=__embuild_traverse_emfile_%)

.PHONY: all
.PHONY: $(__embuild_traverse_emfile_entity_targets)
all: $(__embuild_traverse_emfile_entity_targets)

#$(__embuild_traverse_entity_types:%=__embuild_traverse_emfile_%-%):
$(__embuild_traverse_emfile_entity_targets): __embuild_traverse_emfile_module-%:
	$(MAKE) -f mk/embuild/traverse/entity.mk all \
		__EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_TYPE = module

__embuild_traverse_emfile_%: \
  export __EMBUILD_TRAVERSE_ENTITY_FILE = $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE)

$(__embuild_traverse_emfile_entity_targets): \
  export __EMBUILD_TRAVERSE_ENTITY = $(value $(@:__embuild_traverse_emfile_%=%))


