#
#   Date: Jun 14, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
# 	- modules sorted according dependencies and runlevel
#

include mk/script/c-runtime-inject.mk

mod_inst_runlevel=$(or $(call annotation_value,$(call get,$m,includeMember),$(my_rl_value)),2)

_mod_inst_closure= \
	$(if $(strip $2),\
		$(foreach d,$(firstword $2),\
			$(call _mod_inst_closure,\
				$1 $d,\
				$(filter-out $1 $d,$(sort $(call mod_inst_get_deps,$d,depends) $(call nofirstword,$2))))),$1)
mod_inst_closure=$(call _mod_inst_closure,,$1)

runlevel_modules_closure=$(strip $(foreach m,$(suffix $(modules)),\
			 $(if $(findstring $1,$(call mod_inst_runlevel,$m)),\
				 $(call mod_inst_closure,$m))))

val_or_null = $(if $1,$1,generic__notexisting)

seq := $(shell $(SEQ) 0 2000)

backslash := \#

# Call info for every new module in this runlevel
# 1. Runlevels to generate
# 2. Already loaded modules
# 3. Modules to be loaded at this runlevel
_gen_mod_info = \
	$(foreach m,$3, \
		$(info $(backslash)define __mod_runlevel__$(call fqn2id,$(call mod_inst_fqn,$m)) $(firstword $1))) \
	$(call gen_mod_info,$(call nofirstword,$1),$2 $3)

_mod_inst_get_deps=$(call mod_inst_get_deps,$1,depends)

# Generate modules defs in right order according runlevel and dependencies
# 1. List of runlevels to generated
# 2. Already `loaded` modules (already generated)
gen_mod_info = \
	$(if $(strip $1), \
		$(info /* Module runlevels */) \
		$(call _gen_mod_info,$1,$2, \
			$(filter-out $2, \
				$(call topsort,$(sort $(call runlevel_modules_closure,$(firstword $1))), \
					_mod_inst_get_deps))), \
		$(info ) \
		$(info /* Module priorities */) \
		$(foreach m,$(call minjoin,$2,$(addprefix .,$(seq))), \
			$(info $(backslash)define __mod_priority__$(call fqn2id,$(call mod_inst_fqn,$(basename $m))) 1$(subst .,,$(suffix $m)))) \
		$(info ) \
		$(info /* Module names */) \
		$(foreach m,$2,$(foreach n,$(call mod_inst_fqn,$m), \
			$(info $(backslash)define __mod_name__$(call fqn2id,$n) "$(subst .,,$(suffix $n))"))) \
		$(info ) \
		$(info /* Module packages */) \
		$(foreach m,$2,$(foreach n,$(call mod_inst_fqn,$m), \
			$(info $(backslash)define __mod_package__$(call fqn2id,$n) "$(basename $n)"))) \
		$(info ) \
		$(info /* Number of modules */) \
		$(info $(backslash)define __MOD_COUNT $(words $2)))

#
# The output.
#

$(info /* $(GEN_BANNER) */)
$(info )

$(info #ifndef GEN_SRC_FRAMEWORK_MOD_INFO_H_)
$(info #define GEN_SRC_FRAMEWORK_MOD_INFO_H_)
$(info )

$(call gen_mod_info,0 1 2 3 4,)
$(info )

$(info #endif /* GEN_SRC_FRAMEWORK_MOD_INFO_H_ */)
$(info )
