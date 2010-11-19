#
# Traverser sandbox for em-files. Entry point for sub-make.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

# Only essential scripts are included here to avoid possible collisions
# with variables of sandboxed file.
include util/envdef.mk
include util/sandbox.mk

# Check the needs.
$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_EMFILE_ROOT \
  __EMBUILD_TRAVERSE_EMFILE      \
,Traverser em-file sandbox needs these variables to be defined)

# Go!
#emfile_variables := $(call sandbox, \
  include $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE) \
)

# Process cathed variables.
include embuild/traverse/emfile.mk
