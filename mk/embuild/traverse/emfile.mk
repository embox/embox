#
# Traverser sandbox for em-files.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

__sandbox_variables := __sandbox_variables $(.VARIABLES)
include $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE)
__sandbox_variables := $(filter-out $(__sandbox_variables),$(.VARIABLES))

emfile_variables := $(__sandbox_variables)

variables := $(.VARIABLES)

include gmsl.mk
include util/common.mk
include util/math.mk
include embuild/traverse/entity.mk

dollar_encode = $(subst $$,$$$$,$1)
dollar_decode = $(subst $$$$,$$,$1)

variables_seq := $(call int_seq,x,$(variables:%=x))
variables_esc := $(call dollar_encode,$(variables))

variable_pairs = \
  $(call pairmap,__variable_pairs_cb,x $(variables_seq),$(variables_seq) x)

# 1. i
# 2. i+1
__variable_pairs_cb = $(call whitespace_escape,$ \
  $(if $(findstring x,$1),$ \
       $(firstword $(variables_esc)),$ \
       $(if $(findstring x,$2),$ \
            $(lastword $(variables_esc)),$ \
            $(wordlist $1,$2,$(variables_esc))$ \
        )$ \
   )$ \
)

whitespace_escape = \
  $(subst $(\space),_$$s,$(subst $(\t),_$$t,$(subst $(\n),_$$n,$1)))
whitespace_unescape = \
  $(subst _$$s,$(\space),$(subst _$$t,$(\t),$(subst _$$n,$(\n),$1)))

check_defined = $(if $(findstring undefined,$(flavor $1)),,$(info $1))
$(foreach p,$(variable_pairs),$(call check_defined,$(call whitespace_unescape,$p)))

pairs_escape = $(call __pairs_escape,$(subst $$,$$$$,$1))
__pairs_escape = $(call pairmap,__pairs_escape_cb,_$$_$$_ $1,$1 _$$_$$_)
__pairs_escape_cb = $1_$$_$2

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


