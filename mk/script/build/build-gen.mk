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
	set_on_error_trap() { trap "$$1" INT QUIT TERM HUP EXIT; };            \
	COMMAND=$(call sh_quote,set_on_error_trap "$(RM) $$OUTFILE"; { $2; }); \
	OUTFILE=$(call trim,$1);                                               \
	if [ ! -f $$OUTFILE ];                                                 \
	then                                                                   \
		__OUTDIR=`dirname $$OUTFILE`;                                      \
		{ [ -d $$__OUTDIR ] || $(MKDIR) $$__OUTDIR; }                      \
			&& eval "$$COMMAND" && set_on_error_trap -;                    \
	else                                                                   \
		__OUTFILE=$$OUTFILE; OUTFILE=$${TMPDIR:-/tmp}/Mybuild.$$$$;        \
		eval "$$COMMAND"                                                   \
			&& { cmp -s $$OUTFILE $$__OUTFILE >/dev/null 2>&1              \
					|| { $(MV) $$OUTFILE $$__OUTFILE                       \
							&& set_on_error_trap -; }; };                  \
	fi

#cmd_notouch = \
	OUTFILE=$(call trim,$1); { $2; }

#   1. Output file.
#   2. The command which outputs its result to stdout.
cmd_notouch_stdout = \
	$(call cmd_notouch,$1,{ $2; } > $$OUTFILE)

sh_quote = \
	'$(subst ','\'',$1)'
fmt_line = \
	$(if $2,$1)$(subst $(\n),$(\n)$1,$2)

# 1. Text to comment.
gen_comment = \
	$(PRINTF) '%s\n\n' \
		$(call sh_quote,$(call fmt_line,$(\h) ,$1))

gen_banner = \
	$(call gen_comment,$(GEN_BANNER))

# 1. Variable name.
# 2. Value.
gen_make_var = \
	$(PRINTF) '%s := %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2)

# 1. Target.
# 2. Prerequisites.
# 3. Recipe.
gen_make_rule = \
	$(PRINTF) '%s : %s\n%s\n\n' \
		$(call sh_quote,$1)     \
		$(call sh_quote,$2)     \
		$(call sh_quote,$(call fmt_line,$(\t),$3))

# 1. Target.
# 2. Prerequisites.
gen_make_dep = \
	$(PRINTF) '%s : %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2)

# 1. Target.
# 2. Variable name.
# 3. Value.
gen_make_tsvar = \
	$(PRINTF) '%s : %s := %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(call sh_quote,$3)

# 1. Target.
# 2. Variable name.
# 3. Value (assumed to be a list).
gen_make_tsvar_list = \
	$(PRINTF) '%s : %s := $(foreach ,$3,\\\n\t\t%s)\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(foreach v,$3,$(call sh_quote,$v))

# Working with these lists...

build_modules := \
	$(call get,$(build_model),modules)
build_sources := \
	$(call get,$(build_modules),sources)

#
# Global artifacts.
#

@build_image := \
	$(build_model:%=build-image-rule-mk/%)

@build_all = \
	$(@build_image)

#all .PHONY : $(@build_all)

build_image_rulemk_mk_pat = $(MKGEN_DIR)/%.rule.mk

$(@build_image) : filename = image
$(@build_image) : @file    = $(filename:%=$(build_image_rulemk_mk_pat))

$(@build_image) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,module_path,$(path)); \
		$(call gen_make_dep,$(a_file),$(mk_file)); \
		$(call gen_make_tsvar_list,$(a_file),ar_objects,$(objs)))

#
# Per-module artifacts.
#

@module_h := $(build_modules:%=module-h/%)
@module_ar_rulemk := $(patsubst %,module-ar-rule-mk/%, \
	$(call filter_static_modules,$(build_modules)))

@module_all = \
	$(@module_h) \
	$(@module_ar_rulemk)

all .PHONY : $(@module_ar_rulemk)

module_fqn  = $(call get,$(call get,$1,type),qualifiedName)
module_path = $(subst .,/,$(fqn))

$(@module_all) : fqn   = $(call module_fqn,$@)
$(@module_all) : path  = $(call module_path,$@)

module_ar_rulemk_mk_pat = $(MKGEN_DIR)/%.rule.mk
module_ar_rulemk_a_pat  = $(OBJ_DIR)/%.a

$(@module_ar_rulemk) : @file   = $(path:%=$(module_ar_rulemk_mk_pat))
$(@module_ar_rulemk) : mk_file = $(patsubst %,$(value module_ar_rulemk_mk_pat),$$(module_path))
$(@module_ar_rulemk) : a_file  = $(patsubst %,$(value module_ar_rulemk_a_pat),$$(module_path))
$(@module_ar_rulemk) : objs    = $(call module_get_objects,$@)

$(@module_ar_rulemk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,module_path,$(path)); \
		$(call gen_make_dep,$(a_file),$(mk_file)); \
		$(call gen_make_tsvar_list,$(a_file),ar_objects,$(objs)))

$(@module_h) : h_file = $(SRCGEN_DIR)/include/module/$(path).h

$(@module_h) :
	@$(call cmd_notouch_stdout,$(h_file), \
		$(MAKE) -f mk/script/h-genmodule.mk)

#
# Per-source artifacts.
#

source_member = $(call invoke,$1,eContainer)
source_annotation_values = \
	$(call invoke,$(source_member),getAnnotationValuesOfOption,$2)

my_gen_script := $(call mybuild_resolve_or_die,mybuild.lang.Generated.script)

@source_gen := \
	$(foreach s,$(build_sources), \
		$(patsubst %,source-gen/%$s, \
			$(call source_annotation_values,$s,$(my_gen_script))))

@source_rulemk := \
	$(foreach s,$(build_sources), \
		$(foreach f,$(call get,$s,fileName), \
			$(if $(filter %.S %.c %.cpp %.cxx,$f),source-rule-mk/$s)))

@source_all = \
	$(@source_gen) \
	$(@source_rulemk)

all .PHONY : $(@source_all)

#m_ar :$(@source_all)
#	@$(call cmd_notouch_stdout,test, \
#		$(call gen_make_tsvar_list,target,objs,foo bar))

source_file = $(call get,$1,fileFullName)
source_base = $(basename $(source_file))

$(@source_all) : file = $(call source_file,$@)
$(@source_all) : base = $(call source_base,$@)

$(@source_all) : values_of = $(call source_annotation_values,$@,$1)

my_defmacro_val := $(call mybuild_resolve_or_die,mybuild.lang.DefineMacro.value)
my_incpath_val  := $(call mybuild_resolve_or_die,mybuild.lang.IncludePath.value)

$(@source_rulemk) : includes = $(call values_of,$(my_incpath_val))
$(@source_rulemk) : defines  = $(call values_of,$(my_defmacro_val))

$(@source_rulemk) : do_flags = $(foreach f,$2,$1$(call sh_quote,$(call get,$f,value)))
$(@source_rulemk) : flags = $(call trim, \
		$(call do_flags,-I,$(includes)) \
		$(call do_flags,-D,$(defines)))

source_rulemk_mk_pat = $(MKGEN_DIR)/%.rule.mk
source_rulemk_o_pat  = $(OBJ_DIR)/%.o

$(@source_rulemk) : @file   = $(base:%=$(source_rulemk_mk_pat))
$(@source_rulemk) : mk_file = $(patsubst %,$(value source_rulemk_mk_pat),$$(source_base))
$(@source_rulemk) : o_file  = $(patsubst %,$(value source_rulemk_o_pat),$$(source_base))

$(@source_rulemk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,source_base,$(base)); \
		$(call gen_make_dep,$(o_file),$(mk_file)); \
		$(call gen_make_tsvar,$(o_file),flags,$(flags)))

$(@source_gen) : @file = $(SRCGEN_DIR)/$(file)
$(@source_gen) : script = $(call expand,$(call get,$(basename $@),value))

$(@source_gen) :
# Plus sign is for case when generator script is also Make-based.
# Avoids 'jobserver unavailable' warning for parallel builds.
	+@$(call cmd_notouch_stdout,$(@file), \
		$(script))

