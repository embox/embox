#
#   Date: Jun 14, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

mod_inst_get_deps=$(filter-out $(call get,$1,noRuntimeDepends),$(call get,$1,$2))

_mod_inst_get_deps=$(call mod_inst_get_deps,$1,depends)

_build_modules:=$(call get,$(build_model),modules)
_build_modules_topsorted:=$(call topsort,$(_build_modules),_mod_inst_get_deps)

ifneq ($(words $(_build_modules)),$(words $(_build_modules_topsorted)))
$(error error $(_build_modules) $(\n)$(words $(_build_modules))$(\n) $(_build_modules_topsorted)$(\n)$(words $(_build_modules_topsorted))$(\n) $(filter-out $(_build_modules_topsorted),$(_build_modules)))
endif

mod_inst_fqn=$(call get,$(call get,$1,type),qualifiedName)
# f.q.n.obj
modules := \
	$(foreach m,$(_build_modules_topsorted), \
		$(call get,$(call get,$m,type),qualifiedName)$m)

# f.q.n
packages := \
	$(sort generic $(basename $(basename $(modules))))

my_app       := $(call mybuild_resolve_or_die,mybuild.lang.App)
my_cmd       := $(call mybuild_resolve_or_die,mybuild.lang.Cmd)
my_cmd_name  := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.name)
my_cmd_help  := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.help)
my_cmd_man   := $(call mybuild_resolve_or_die,mybuild.lang.Cmd.man)
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

app_modules := \
	$(foreach m,$(modules),$(if $(call is_a,$(my_app),$m),$m))


str_escape = \
	"$(subst $(\n),\n"$(\n)$(\t)",$(subst $(\t),\t,$(subst ",\",$(subst \,\\,$1))))"

fqn2id = $(subst .,__,$1)

# 1. Module
# 2. depends/afterDepends
get_deps = \
	$(call get,$(call get,$(call module_instance_get_deps,$1,$2),type),qualifiedName)

mod_inst_runlevel=$(or $(call annotation_value,$(call get,$m,includeMember),$(my_rl_value)),2)

mod_inst_closure=$1 $(foreach d,$(call mod_inst_get_deps,$1,depends),$(call mod_inst_closure,$d))

runlevel_modules_closure=$(strip $(foreach m,$(suffix $(modules)),\
			 $(if $(findstring $1,$(call mod_inst_runlevel,$m)),\
				 $(call mod_inst_closure,$m))))

mod_def = \
	MOD_DEF($(call fqn2id,$(basename $1)), $(subst .,,$(suffix $1)));

# Call info for every new module in this runlevel
# 1. Runlevels to generate
# 2. Already loaded modules
# 3. Modules to be loaded at this runlevel
_gen_mod_runlevels = $(foreach m,$3,$(info $(call mod_def,$(call mod_inst_fqn,$m)))) \
	$(call gen_mod_runlevels,$(call nofirstword,$1),$2 $3)

# Generate modules defs in right order according runlevel and dependencies
# 1. List of runlevels to generated
# 2. Already `loaded` modules (already generated)
gen_mod_runlevels = \
	$(if $(strip $1), \
		$(info /* Runlevel $(firstword $1) */) \
		$(call _gen_mod_runlevels,$1,$2,$(filter-out $2, \
			$(call topsort,$(sort $(call runlevel_modules_closure,$(firstword $1))),_mod_inst_get_deps))))

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
$(call gen_mod_runlevels,0 1 2 3,)
$(info )

$(info /* Runlevel modules. */)
$(foreach n,$(addprefix generic.runlevel,0 1 2 3),$(info $(call mod_def,$n)))
$(info )
