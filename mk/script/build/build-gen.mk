#
#   Date: Apr 25, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

# Wraps the given rule with a script which compares the command output with
# the original file (if it exists) and replaces the latter only in case when
# contents differ.
#   1. The complete command which should output its result to the temporary
#      file specified in the '$TMPFILE' environment variable.
#   2. Target file.
rule_notouch = \
	TMPFILE=`mktemp -qt Mybuild.XXXXXXXX` && {         \
		trap '$(RM) $$TMPFILE' INT QUIT TERM HUP EXIT; \
		$1 && {                                        \
			cmp -s $$TMPFILE $2 >/dev/null 2>&1        \
				|| $(MV) $$TMPFILE $2 && trap - EXIT;  \
		}                                              \
	}

rule_notouch_stdout = \
	$(call rule_notouch,$1 > $$TMPFILE,$2)

build_modules := \
	$(call get,$(build_model),modules)
build_sources := \
	$(call get,$(build_modules),sources)

all .PHONY : $(build_model:%=build-runtime-inject-c/%)
all .PHONY : $(build_modules:%=module/%)
all .PHONY : $(build_sources:%=source/%)

$(build_model:%=build-runtime-inject-c/%) : build-runtime-inject-c/% :
	@$(call rule_notouch_stdout, \
		$(MAKE) -f mk/script/depsinject.mk,$(SRC_DIR)/depsinject.c)

$(build_modules:%=module/%) : module/% : module-h/%

module-h/% :
	@$(call rule_notouch_stdout, \
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
	@$(foreach f,$(call get,$*,fileFullName),$(call rule_notouch_stdout, \
		echo '$(call flags,$f) -o $(basename $f).o -c',$f.cmd))
source-cmd-cc/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$1),C,AS)FLAGS)

source-cmd-cpp/% :


