#
#   Date: Jun 14, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

# f.q.n.obj
modules := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(call get,$(call get,$m,type),qualifiedName)$m)

# f.q.n
packages := \
	$(sort generic $(basename $(basename $(modules))))

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

str_escape = \
	"$(subst $(\n),\n,$(subst $(\t),\t,$(subst ",\",$(subst \,\\,$1))))"

fqn2id = $(subst .,__,$1)

# 1. Module
# 2. depends/afterDepends
get_deps = \
	$(call get,$(call get,$(filter-out $(call get,$1,noRuntimeDepends), \
		$(call get,$1,$2)),type),qualifiedName)

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
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(info MOD_DEF($(call fqn2id,$n), $(call fqn2id,$(basename $n)), \
		"$(subst .,,$(suffix $n))", \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cmd_help))), \
		$(call str_escape,$(call module_annotation_value,$m,$(my_cmd_man))));)))
$(info )

$(info /* Runlevel modules. */)
$(foreach n,$(addprefix generic.runlevel,0 1 2 3), \
	$(info MOD_DEF($(call fqn2id,$n), $(call fqn2id,$(basename $n)), "$n", "", "");))
$(info )

$(info /* Dependencies. */)
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(foreach d,$(call get_deps,$m,depends), \
		$(info MOD_DEP_DEF($(call fqn2id,$n), $(call fqn2id,$d));)) \
	$(foreach d,$(call get_deps,$m,afterDepends), \
		$(info MOD_AFTER_DEP_DEF($(call fqn2id,$n), $(call fqn2id,$d));)) \
	$(foreach r,generic.runlevel$(or $(call annotation_value, \
					$(call get,$m,includeMember),$(my_rl_value)),2), \
		$(info MOD_CONTENTS_DEF($(call fqn2id,$r), $(call fqn2id,$n));))))
$(info )

