#
# Traverser sandbox for em-files. Entry point for sub-make.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

# Only essential scripts are included here to avoid possible collisions
# with variables of sandboxed file.
include util/envdef.mk

# Check the needs.
$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_EMFILE_ROOT \
  __EMBUILD_TRAVERSE_EMFILE      \
,Traverser em-file sandbox needs these variables to be defined)

__emfile := $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE)

__emfile_sandbox_variables_before :=
__emfile_sandbox_variables_before := $(.VARIABLES)

# Go!
include $(__emfile)

__emfile_sandbox_variables_after  := $(.VARIABLES)

# Process catched variables.
include embuild/traverse/emfile.mk

emfile_sandbox_result := $(emfile_handle_all)

#emfile_sandbox_result_errors := \
#  $(call emfile_errors_filter,$(emfile_sandbox_result))
#emfile_sandbox_result_entities := \
#  $(call emfile_entities_filter,$(emfile_sandbox_result))

emfile_sandbox_target_pattern := emfile_sandbox_target-%

emfile_sandbox_entity_target_pattern := \
  $(subst %,$(emfile_entity_pattern),$(emfile_sandbox_target_pattern))
emfile_sandbox_error_target_pattern := \
  $(subst %,$(emfile_error_pattern),$(emfile_sandbox_target_pattern))

emfile_sandbox_targets := \
  $(emfile_sandbox_result:%=$(emfile_sandbox_target_pattern))

emfile_sandbox_entity_targets := \
  $(filter $(emfile_sandbox_entity_target_pattern),$(emfile_sandbox_targets))
emfile_sandbox_error_targets := \
  $(filter $(emfile_sandbox_error_target_pattern),$(emfile_sandbox_targets))

.PHONY: all
.PHONY: $(emfile_sandbox_targets)
all: $(emfile_sandbox_targets)

$(emfile_sandbox_entity_targets): $(emfile_sandbox_entity_target_pattern):
	$(MAKE) -f mk/embuild/traverse/entity_sandbox.mk all

# TODO
emfile_entity_full_name = $(subst /,.,$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*)
$(emfile_sandbox_entity_target_pattern): \
  export __EMBUILD_TRAVERSE_ENTITY = $(emfile_entity_full_name)

$(emfile_sandbox_entity_target_pattern): \
  export __EMBUILD_TRAVERSE_ENTITY_FILE := $(__emfile)

$(emfile_sandbox_entity_target_pattern): \
  export __EMBUILD_TRAVERSE_ENTITY_VALUE = $($(call emfile_entity,$*))

$(emfile_sandbox_error_targets): $(emfile_sandbox_error_target_pattern):
	@echo $($*)

# TODO exit with an error
all: ;

