#
#   Date: Jun 14, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
# 	- modules sorted according dependencies and runlevel
#

include mk/script/c-runtime-inject.mk

str_escape = \
	"$(subst $(\n),\n"$(\n)$(\t)",$(subst $(\t),\t,$(subst ",\",$(subst \,\\,$1))))"

get_deps = \
	$(call get,$(call get,$(call mod_inst_get_deps,$1,$2),type),qualifiedName)

#
# The output.
#

$(info /* $(GEN_BANNER) */)
$(info )

$(info #include <framework/mod/embuild.h>)
$(info )

$(info /* Security labels. */)
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(info MOD_LABEL_DEF($(call fqn2id,$n));)))
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
		$(info MOD_DEP_DEF($(call fqn2id,$n), $(call fqn2id,$d));))))
$(info )

$(info /* Module build information. */)
$(foreach m,$(modules),$(foreach n,$(basename $m), \
	$(info MOD_DEF($(call fqn2id,$n));)))
$(info )
