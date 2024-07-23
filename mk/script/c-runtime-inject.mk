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

fqn2id = $(subst .,__,$1)

mod_inst_get_deps=$(filter-out $(call get,$1,noRuntimeDepends),$(call get,$1,$2))
