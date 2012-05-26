#
#   Date: Apr 25, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

# Wraps the given rule with a script which compares the command output with
# the original file (if it exists) and replaces the latter only in case when
# contents differ.
#   1. Output file.
#   2. The complete command which should output its result to the temporary
#      file specified in the '$OUTFILE' environment variable.
cmd_notouch = \
	COMMAND='{ $2 }'; OUTFILE=$(call trim,$1);                     \
	$(eval_command)

eval_command = \
	if [ ! -f $$OUTFILE ];                                         \
	then                                                           \
		__OUTDIR=`basename $$OUTFILE`;                             \
			[ -d $$__OUTDIR ] || $(MKDIR) $$__OUTDIR;              \
	else                                                           \
		__COMMAND=$$COMMAND; __OUTFILE=$$OUTFILE;                  \
		COMMAND='$$__COMMAND                                       \
			&& { cmp -s $$OUTFILE $$__OUTFILE >/dev/null 2>&1      \
				|| $(MV) $$OUTFILE $$__OUTFILE && trap - EXIT; }   \
			|| exit; COMMAND=$$__COMMAND; OUTFILE=$$__OUTFILE;';   \
		OUTFILE=`mktemp -qt Mybuild.XXXXXXXX`                      \
			&& trap "$(RM) $$OUTFILE" INT QUIT TERM HUP EXIT;      \
	fi                                                             \
		&& eval $$COMMAND

#   1. Output file.
#   2. The command which outputs its result to stdout.
cmd_notouch_stdout = \
	$(call cmd_notouch,$1,$2 > $$OUTFILE)

sh_quote = \
	'$(subst ','"'"',$1)'
fmt_line = \
	$1$(subst $(\n),$(\n)$1,$2)

# 1. Text to comment.
gen_comment = \
	$(PRINTF) '%s\n\n' \
		$(call sh_quote,$(call fmt_line,$(\h) ,$1))

# 1. Target.
# 2. Prerequisites.
# 3. Recipe.
gen_make_rule = \
	$(PRINTF) '%s : %s\n%s\n\n' \
		$(call sh_quote,$1)     \
		$(call sh_quote,$2)     \
		$(call sh_quote,$(call fmt_line,$(\t),$3))

#
# Global artifacts.
#

build_model_targets := \
	$(build_model:%=build-runtime-inject-c/%) \
	$(build_model:%=build-cmd-ld-image/%)

all .PHONY : $(build_model_targets)

$(filter build-runtime-inject-c/%,$(build_model_targets)) : build-runtime-inject-c/% :
	@$(call cmd_notouch_stdout,$(SRC_DIR)/depsinject.c, \
		$(MAKE) -f mk/script/depsinject.mk)

$(filter build-cmd-ld-image/%,$(build_model_targets)) : build-cmd-ld-image/% :
	@$(foreach f,$(call get,$*,fileFullName),$(call cmd_gen_make_rule,$f., \
		echo '$(call flags,$f) -o $(basename $f).o -c'))
build-cmd-ld-image/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$1),C,AS)FLAGS)

#
# Per-module artifacts.
#

build_modules := \
	$(call get,$(build_model),modules)
module_targets := \
	$(build_modules:%=module-h/%)

all .PHONY : $(module_targets)

$(module_targets) : module-h/% :
	@$(call cmd_notouch_stdout, \
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
	@$(foreach f,$(call get,$*,fileFullName),$(call cmd_notouch_stdout, \
		echo '$(call flags,$f) -o $(basename $f).o -c',$f.cmd))
source-cmd-cc/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$1),C,AS)FLAGS)

$(filter source-cc-mk/%,$(source_targets)) : source-cc-mk/% :
	@$(foreach f,$(call get,$*,fileFullName),$(call cmd_notouch_stdout, \
		echo '$(call flags,$f) -o $(basename $f).o -c',$f.cmd))
source-cmd-cc/% : flags = \
	$(CPPFLAGS) $($(if $(filter %.c,$1),C,AS)FLAGS)

$(filter source-cmd-cpp/%,$(source_targets)) : source-cmd-cpp/% :


