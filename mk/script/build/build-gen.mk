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

#
# Global artifacts.
#

build_model_targets := \
	$(build_model:%=build-runtime-inject-c/%)

all .PHONY : $(build_model_targets)

$(build_model_targets) : build-runtime-inject-c/% :
	@$(call rule_notouch_stdout, \
		$(MAKE) -f mk/script/depsinject.mk,$(SRC_DIR)/depsinject.c)

#
# Per-module artifacts.
#

build_modules := \
	$(call get,$(build_model),modules)
module_targets := \
	$(build_modules:%=module-h/%)

all .PHONY : $(module_targets)

$(module_targets) : module-h/% :
	@$(call rule_notouch_stdout, \
		$(MAKE) -f mk/script/h-genmodule.mk,$(call module_h,$*))
module_h = \
	$(call fqn_to_h,$(call get,$(call get,$1,type),qualifiedName))

fqn_to_h = $(SRC_DIR)/include/module/$(fqn_to_dir).h

fqn_to_dir = $(subst .,/,$1)#< 'qualified.name' -> 'qualified/name'
fqn_to_id = $(subst .,__,$1)#< 'qualified.name' -> 'qualified__name'

#
# Per-source artifacts.
#

build_sources := \
	$(call get,$(build_modules),sources)
source_targets := \
	$(foreach s,$(build_sources), \
		$(foreach f,$(call get,$s,fileName), \
			$(if $(filter %.c %.S,$f),source-cmd-cc/$s)  \
			$(if $(filter %.lds.S,$f),source-cmd-cpp/$s)))

all .PHONY : $(source_targets)

$(filter source-cmd-cc/%,$(source_targets)) : source-cmd-cc/% :
	@$(foreach f,$(call get,$*,fileFullName),$(call rule_notouch_stdout, \
		echo '$(call flags,$f) -o $(basename $f).o -c',$f.cmd))
source-cmd-cc/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$1),C,AS)FLAGS)

$(filter source-cmd-cpp/%,$(source_targets)) : source-cmd-cpp/% :


