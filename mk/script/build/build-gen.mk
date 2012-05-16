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

all .PHONY : $(build_model:%=genbuild-runtime-inject-c/%)
all .PHONY : $(build_modules:%=genmodule-h/%)
all .PHONY : $(build_sources:%=gensource-compile-cmd/%)

$(build_modules:%=genmodule-h/%) : genmodule-h/% :
	@$(call rule_cmp_mv, \
		$(MAKE) -f mk/script/h-genmodule.mk,$(call genmodule_h,$*))
genmodule_h = \
	$(call fqn_to_h,$(call get,$(call get,$1,type),qualifiedName))

$(build_model:%=genbuild-runtime-inject-c/%) : genbuild-runtime-inject-c/% :
	@$(call rule_cmp_mv, \
		$(MAKE) -f mk/script/depsinject.mk,$(SRCGEN_DIR)/depsinject.c)

$(build_sources:%=gensource/%) : gensource/% :

fqn_to_h = $(SRCGEN_DIR)/include/module/$(fqn_to_dir).h

fqn_to_dir = $(subst .,/,$1)#< 'qualified.name' -> 'qualified/name'
fqn_to_id = $(subst .,__,$1)#< 'qualified.name' -> 'qualified__name'

