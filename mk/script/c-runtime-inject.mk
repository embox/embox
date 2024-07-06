#
#   Date: Jun 14, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
# 	- modules sorted according dependencies and runlevel
#

include mk/script/script-common.mk

_build_modules:=$(call get,$(build_model),modules)

mod_inst_fqn=$(call get,$(call get,$1,type),qualifiedName)
# f.q.n.obj
modules := \
	$(foreach m,$(_build_modules), \
		$(call get,$(call get,$m,type),qualifiedName)$m)

# f.q.n
packages := \
	$(sort generic $(basename $(basename $(modules))))

my_app       := $(call mybuild_resolve_or_die,mybuild.lang.App)
my_cmd       := $(call mybuild_resolve_or_die,mybuild.lang.Cmd)
my_cmd_name  := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.name)
my_cmd_help  := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.help)
my_cmd_man   := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.man)
my_cxx_cmd       := $(call mybuild_resolve_or_die,mybuild.lang.CxxCmd)
my_cxx_cmd_name  := $(call mybuild_resolve_or_die,mybuild.lang.CxxCmd.name)
my_cxx_cmd_help  := $(call mybuild_resolve_or_die,mybuild.lang.CxxCmd.help)
my_cxx_cmd_man   := $(call mybuild_resolve_or_die,mybuild.lang.CxxCmd.man)
my_rl_value  := $(call mybuild_resolve_or_die,mybuild.lang.Runlevel.value)

# 1. Module instance.
# 2. Option.
module_annotation_value = \
	$(call annotation_value,$(call get,$1,type),$2)

# 1. Annotation target.
# 2. Option.
annotation_value = \
	$(call __single_value_check,$(call invoke,$1,getAnnotationValuesOfOption,$2))
__single_value_check = \
	$(if $(strip $1),$(call assert,$(call singleword,$1))$(call get,$1,value))


is_a = \
	$(strip $(call invoke,$(call get,$2,allTypes),getAnnotationsOfType,$1))

cmd_modules := \
	$(foreach m,$(modules),$(if $(call is_a,$(my_cmd),$m),$m))

cxx_cmd_modules := \
	$(foreach m,$(modules),$(if $(call is_a,$(my_cxx_cmd),$m),$m))

app_modules := \
	$(foreach m,$(modules),$(if $(call is_a,$(my_app),$m),$m))


str_escape = \
	"$(subst $(\n),\n"$(\n)$(\t)",$(subst $(\t),\t,$(subst ",\",$(subst \,\\,$1))))"

fqn2id = $(subst .,__,$1)

mod_inst_get_deps=$(filter-out $(call get,$1,noRuntimeDepends),$(call get,$1,$2))

# 1. Module
# 2. depends/afterDepends
get_deps = \
	$(call get,$(call get,$(call mod_inst_get_deps,$1,$2),type),qualifiedName)

mod_inst_runlevel=$(or $(call annotation_value,$(call get,$m,includeMember),$(my_rl_value)),2)

_mod_inst_closure= \
	$(if $(strip $2),\
		$(foreach d,$(firstword $2),\
			$(call _mod_inst_closure,\
				$1 $d,\
				$(filter-out $1 $d,$(sort $(call mod_inst_get_deps,$d,depends) $(call nofirstword,$2))))),\
		$1)
mod_inst_closure=$(call _mod_inst_closure,,$1)

runlevel_modules_closure=$(strip $(foreach m,$(suffix $(modules)),\
			 $(if $(findstring $1,$(call mod_inst_runlevel,$m)),\
				 $(call mod_inst_closure,$m))))

mod_def = \
	MOD_DEF($1, $2, $(call fqn2id,$(basename $3)), "$(basename $3)", $(subst .,,$(suffix $3)));

val_or_null = $(if $1,$1,generic__notexisting)

seq := $(shell $(SEQ) 1 1000)

# Call info for every new module in this runlevel
# 1. Runlevels to generate
# 2. Already loaded modules
# 3. Modules to be loaded at this runlevel
_gen_mod_runlevels = \
	$(foreach m,$(call minjoin,$3,$(addprefix .,$(seq))), \
		     $(info $(call mod_def,$(firstword $1),$(subst .,,$(suffix $m)),$(call mod_inst_fqn,$(basename $m))))) \
	$(call gen_mod_runlevels,$(call nofirstword,$1),$2 $3)

_mod_inst_get_deps=$(call mod_inst_get_deps,$1,depends)

# Generate modules defs in right order according runlevel and dependencies
# 1. List of runlevels to generated
# 2. Already `loaded` modules (already generated)
gen_mod_runlevels = \
	$(if $(strip $1), \
		$(info /* Runlevel $(firstword $1) */) \
		$(call _gen_mod_runlevels,$1,$2, \
			$(filter-out $2, \
				$(call topsort,$(sort $(call runlevel_modules_closure,$(firstword $1))), \
					_mod_inst_get_deps))))

#
# The output.
#

$(info /* $(GEN_BANNER) */)
$(info )

$(foreach h, framework/mod/embuild.h, \
	$(info #include <$h>))
$(info )


$(info /* Package definitions. */)
$(foreach p,$(packages), \
	$(info MOD_PACKAGE_DEF($(call fqn2id,$p), "$p");))
$(info )

$(info /* Module definitions. */)
$(call gen_mod_runlevels,0 1 2 3 4,)
$(info )

$(info /* Security labels. */)
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(info MOD_LABEL_DEF($(call fqn2id,$n));) \
	$(info MOD_SEC_LABEL_DEF($(call fqn2id,$n));)))
$(info )

$(info /* Loggers. */)
_gen_logger_def = \
	$(if $1,$(info MOD_LOGGER_DEF($(call fqn2id,$n), \
		$(or $1,0));))
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(call _gen_logger_def,$(call invoke,$m,getOptionValueStringByName,log_level))))
$(info )

$(info /* Applications. */)
$(foreach m,$(app_modules),$(foreach n,$(basename $m), \
	$(info MOD_APP_DEF($(call fqn2id,$n));)))
$(info )

$(info /* Commands. */)
$(foreach m,$(cmd_modules),$(foreach n,$(basename $m), \
	$(info MOD_CMD_DEF($(call fqn2id,$n), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cmd_name))), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cmd_help))), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cmd_man))));)))
$(info )

$(info /* C++ commands. */)
$(foreach m,$(cxx_cmd_modules),$(foreach n,$(basename $m), \
	$(info MOD_CMD_DEF($(call fqn2id,$n), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cxx_cmd_name))), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cxx_cmd_help))), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cxx_cmd_man))));)))
$(info )

$(info /* Dependencies. */)
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(foreach d,$(call get_deps,$m,depends), \
		$(info MOD_DEP_DEF($(call fqn2id,$n), $(call fqn2id,$d));)) \
	$(foreach d,$(call get_deps,$m,afterDepends), \
		$(info MOD_AFTER_DEP_DEF($(call fqn2id,$n), $(call fqn2id,$d));))))
$(info )

