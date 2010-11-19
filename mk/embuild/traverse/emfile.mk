#
# Traverser sandbox for em-files.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

include util/envdef.mk
include util/sandbox.mk

$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_EMFILE_ROOT \
  __EMBUILD_TRAVERSE_EMFILE      \
,Traverser em-file sandbox needs these variables to be defined)

# Go!
__embuild_traverse_emfile_variables := $(call sandbox, \
  include $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE) \
)

# Now we are free to include some other files.
include embuild/traverse/entity.mk

__embuild_traverse_emfile_entity_variables := \
  $(filter $(__embuild_traverse_entity_types:%=%-%), \
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


