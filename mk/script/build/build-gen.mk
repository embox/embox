#
#   Date: Apr 25, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

# Wraps the given rule with a script which compares the command output with
# the original file (if any exists) and replaces the latter only in case when
# contents differ.
#   1. Command to execute.
#   2. Target file.
rule_cmp_mv = \
	$1 > $2.tmp; __EXIT_CODE__=$$?; \
	if [ $$__EXIT_CODE__ -eq 0 ];
	then \
		cmp -s $2 $2.tmp >/dev/null 2>&1 || $(MV) $2.tmp $2; exit;
	else \
		$(RM) $2.tmp; exit $$__EXIT_CODE__
#rule_cmp_and_mv = \
	if $1 > $2.tmp; \
	then \
		if cmp -s $2 $2.tmp >/dev/null 2>&1; \
		then \
			$(RM) $2.tmp; \
		else \
			$(MV) $2.tmp $2; \
	else \
		$(RM) $2.tmp && exit 1

build_modules := \
	$(call get,$(build_model),modules)
build_sources := \
	$(call get,$(build_modules),sources)

all .PHONY : $(build_model:%=build-runtime-inject-c/%)
all .PHONY : $(build_modules:%=module-h/%)
all .PHONY : $(build_sources:%=source-compile-cmd/%)

$(build_model:%=build-runtime-inject-c/%) : build-runtime-inject-c/% :
	@$(call rule_cmp_mv, \
		$(MAKE) -f mk/script/depsinject.mk,$(SRC_DIR)/depsinject.c)

$(build_modules:%=module-h/%) : module-h/% :
	@$(call rule_cmp_mv, \
		$(MAKE) -f mk/script/h-module.mk,$(call module_h,$*))
module_h = \
	$(call fqn_to_h,$(call get,$(call get,$1,type),qualifiedName))

fqn_to_h = $(SRC_DIR)/include/module/$(fqn_to_dir).h

fqn_to_dir = $(subst .,/,$1)#< 'qualified.name' -> 'qualified/name'
fqn_to_id = $(subst .,__,$1)#< 'qualified.name' -> 'qualified__name'

$(build_sources:%=source-/%) : source-/% : $$(source_prerequisites)
source_prerequisites = \
	$(foreach f,$(call get,$*,fileName), \
		$(if $(filter %.c %.S,$f),source-cmd-cc/$*)  \
		$(if $(filter %.lds.S,$f),source-cmd-cpp/$*))

source-cmd-cc/% :
	@$(foreach f,$(call get,$*,fileFullName),$(call rule_cmp_mv, \
		echo '$(flags) -o $(basename $f).o -c',$f.cmd))
source-cmd-cc/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$f),C,AS)FLAGS)

source-cmd-cpp/% :


