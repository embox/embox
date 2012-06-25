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
	$(PRINTF) '%s = %s\n\n' \
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
	$(PRINTF) '%s : %s = %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(call sh_quote,$3)

# 1. Target.
# 2. Variable name.
# 3. Value (assumed to be a list).
gen_make_tsvar_list = \
	$(PRINTF) '%s : %s = $(foreach ,$3,\\\n\t\t%s)\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(foreach v,$3,$(call sh_quote,$v))

# 1. Makefile
# 2. Whether to use silent '-include'.
gen_make_include = \
	$(PRINTF) '%sinclude %s\n\n' \
		$(call sh_quote,$(if $(value 2),-)) \
		$(call sh_quote,$1)

# 1. Makefiles...
# 2. Whether to use silent '-include'.
gen_make_include_list = \
	$(PRINTF) '%sinclude $(foreach ,$1,\\\n\t%s)\n\n' \
		$(call sh_quote,$(if $(value 2),-)) \
		$(foreach v,$1,$(call sh_quote,$v))

# Working with these lists...

build_modules := \
	$(call get,$(build_model),modules)
build_sources := \
	$(foreach m,$(build_modules), \
		$(addprefix $m,$(call get,$m,sources)))

#
# Global artifacts.
#

@build_image := \
	$(build_model:%=build-image-rule-mk/%)

@build_include_mk := \
	$(build_model:%=build-include-mk/%)

@build_all = \
	$(@build_image) \
	$(@build_include_mk)

all .PHONY : $(@build_all)

$(@build_all) : archived_modules = $(suffix $(@module_ar_rulemk))
$(@build_all) : normal_modules = $(filter-out $(archived_modules),$(build_modules))

build_image := image

$(@build_image) : image = $(build_image)

build_image_rulemk_mk_pat     = $(MKGEN_DIR)/%.rule.mk
build_image_rulemk_target_pat = $(BIN_DIR)/%

$(@build_image) : @file    = $(image:%=$(build_image_rulemk_mk_pat))
$(@build_image) : mk_file  = \
		$(patsubst %,$(value build_image_rulemk_mk_pat),$$(build_image))
$(@build_image) : target_file = \
		$(patsubst %,$(value build_image_rulemk_target_pat),$$(build_image))

$(@build_image) : scripts = $(patsubst %,$(value source_cpp_rulemk_o_pat), \
			$(call source_base,$(@source_cpp_rulemk)))
$(@build_image) : objs = $(patsubst %,$(value source_cc_rulemk_o_pat), \
			$(basename $(call module_cc_source_files,$(normal_modules))))
$(@build_image) : libs = $(patsubst %,$(value module_ar_rulemk_a_pat), \
			$(call module_path,$(archived_modules)))

$(@build_image) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,build_image,$(image)); \
		$(call gen_make_dep,$(target_file),$$$$(image_prerequisites)); \
		$(call gen_make_tsvar,$(target_file),mk_file,$(mk_file)); \
		$(call gen_make_tsvar_list,$(target_file),ld_scripts,$(scripts)); \
		$(call gen_make_tsvar_list,$(target_file),ld_objs,$(objs)); \
		$(call gen_make_tsvar_list,$(target_file),ld_libs,$(libs)))

$(@build_include_mk) : @file = $(MKGEN_DIR)/include.mk
$(@build_include_mk) : image_rulemk = \
		$(patsubst %,$(value build_image_rulemk_mk_pat),$(build_image))
$(@build_include_mk) : source_rulemk = \
		$(patsubst %,$(value source_rulemk_mk_pat), \
			$(call source_base,$(@source_rulemk)))
$(@build_include_mk) : module_ar_rulemk = \
		$(patsubst %,$(value module_ar_rulemk_mk_pat), \
			$(call module_path,$(@module_ar_rulemk)))

$(@build_include_mk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_include,$(image_rulemk)); \
		$(call gen_make_include_list,$(source_rulemk)); \
		$(call gen_make_include_list,$(module_ar_rulemk)))

#
# Per-module artifacts.
#

@module_h := \
	$(foreach m,$(build_modules), \
		$(patsubst %,module-h/%$m, \
			$(call get,$m,allTypes)))
@module_ar_rulemk := \
	$(patsubst %,module-ar-rule-mk/%, \
		$(call filter_static_modules,$(build_modules)))

@module_all = \
	$(@module_h) \
	$(@module_ar_rulemk)

all .PHONY : $(@module_all)

module_fqn  = $(call get,$(call get,$1,type),qualifiedName)
module_path = $(subst .,/,$(module_fqn))
module_id   = $(subst .,__,$(module_fqn))

$(@module_all) : fqn   = $(call module_fqn,$@)
$(@module_all) : path  = $(call module_path,$@)

module_ar_rulemk_mk_pat = $(MKGEN_DIR)/%.rule.mk
module_ar_rulemk_a_pat  = $(OBJ_DIR)/%.a

$(@module_ar_rulemk) : @file   = $(path:%=$(module_ar_rulemk_mk_pat))
$(@module_ar_rulemk) : mk_file = $(patsubst %,$(value module_ar_rulemk_mk_pat),$$(module_path))
$(@module_ar_rulemk) : a_file  = $(patsubst %,$(value module_ar_rulemk_a_pat),$$(module_path))

module_cc_source_files = \
	$(filter $(source_cc_pats),$(filter-out $(source_cpp_pats), \
		$(call source_file,$(call get,$1,sources))))

$(@module_ar_rulemk) : objs = $(patsubst %,$(value source_cc_rulemk_o_pat), \
			$(basename $(call module_cc_source_files,$@)))
$(@module_ar_rulemk) : check_objs = $(or $(strip $1), \
	$$(warning static module '$(fqn)' must specify at least one source file.))

$(@module_ar_rulemk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,module_path,$(path)); \
		$(call gen_make_dep,$(a_file),$$$$(ar_prerequisites)); \
		$(call gen_make_tsvar,$(a_file),mk_file,$(mk_file)); \
		$(call gen_make_tsvar_list,$(a_file),ar_objs,$(call check_objs,$(objs))))

$(@module_h) : type = $(basename $@)
$(@module_h) : type_fqn = $(call get,$(type),qualifiedName)
$(@module_h) : type_path = $(subst .,/,$(type_fqn))

module_h_pat = $(SRCGEN_DIR)/include/module/%.h

$(@module_h) : @file = $(type_path:%=$(module_h_pat))
$(@module_h) : printf_escape = $(subst $(\t),\t,$(subst $(\n),\n,$(subst \,\\,$1)))
$(@module_h) : content = $(call __header_template,$@,$(type_fqn))

$(@module_h) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(PRINTF) '%b' $(call sh_quote,$(call printf_escape,$(content))))

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

source_cpp_pats := %.lds.S
source_cc_pats  := %.S %.c %.cpp %.cxx

@source_rulemk := \
	$(foreach s,$(build_sources), \
		$(foreach f,$(call get,$s,fileName), \
			$(if $(filter $(source_cpp_pats),$f),source-cpp-rule-mk/$s, \
				$(if $(filter $(source_cc_pats),$f),source-cc-rule-mk/$s))))

@source_cpp_rulemk := $(filter source-cpp-rule-mk/%,$(@source_rulemk))
@source_cc_rulemk  := $(filter source-cc-rule-mk/%,$(@source_rulemk))

# task/???.module.source
@source_all = \
	$(@source_gen) \
	$(@source_rulemk)

all .PHONY : $(@source_all)

#m_ar :$(@source_all)
#	@$(call cmd_notouch_stdout,test, \
#		$(call gen_make_tsvar_list,target,objs,foo bar))

$(@source_all) : module = $(basename $@)

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
			-include $(patsubst %,$(value module_h_pat),$(call module_path,$(module))) \
			-D__EMBUILD_MOD__=$(call module_id,$(module)) \
			$(call do_flags,-D,$(defines)))

source_rulemk_mk_pat   = $(MKGEN_DIR)/%.rule.mk

$(@source_rulemk) : @file   = $(base:%=$(source_rulemk_mk_pat))
$(@source_rulemk) : mk_file = $(patsubst %,$(value source_rulemk_mk_pat),$$(source_base))
$(@source_rulemk) : prereqs =

source_cc_rulemk_o_pat  = $(OBJ_DIR)/%.o
source_cpp_rulemk_o_pat = $(OBJ_DIR)/%# foo.lds.S -> foo.lds

$(@source_rulemk)  : o_file = $(patsubst %,$(value source_$(kind)_rulemk_o_pat),$$(source_base))

$(@source_rulemk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,source_base,$(base)); \
		$(call gen_make_dep,$(o_file),$$$$($(kind)_prerequisites)); \
		$(call gen_make_tsvar,$(o_file),mk_file,$(mk_file)); \
		$(call gen_make_tsvar,$(o_file),flags,$(flags)); \
		$(call gen_make_include,$$(source_base).d,silent))

$(@source_cc_rulemk)  : kind := cc
$(@source_cpp_rulemk) : kind := cpp

$(@source_gen) : @file = $(SRCGEN_DIR)/$(file)
$(@source_gen) : gen_string = $(basename $(basename $@))
$(@source_gen) : script = $(call expand,$(call get,$(gen_string),value))

$(@source_gen) :
# Plus sign is for case when generator script is also Make-based.
# Avoids 'jobserver unavailable' warning for parallel builds.
	+@$(call cmd_notouch_stdout,$(@file), \
		$(script))

