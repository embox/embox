#
#   Date: Apr 25, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

mod_autocmd_postbuild = \
	$$(EXTERNAL_MAKE_FLAGS) \
	MAIN_STRIPPING_LOCALS=$(if $(strip $1),no,yes) \
	$$(abspath $$(ROOT_DIR))/mk/main-stripping.sh \
	$$(module_id) \
	$$(abspath $$(obj_build)) \
	$$(abspath $$(obj_postbuild))

# Creates shell script with command if it is too big to be passed through
# command line. Must be called from recipe.
#   1. Output file.
#   2. The complete command
cmd_assemble = \
	$(ECHO) '$(\h)!/bin/sh' > $1; \
	$(foreach w,$2,$(ECHO) -n $(strip $(call sh_quote,$w)) ""  >> $1$(\n))

fmt_line = \
	$(if $2,$1)$(subst $(\n),$(\n)$1,$2)

# 1. Text to comment.
gen_comment = \
	$(PRINTF) '%s\n' \
		$(call sh_quote,$(call fmt_line,$(\h) ,$1))

gen_banner = \
	$(PRINTF) '%s\n\n' \
		$(call sh_quote,$(call fmt_line,$(\h) ,$(GEN_BANNER)))

# 1. Variable name.
# 2. Value.
gen_make_var = \
	$(PRINTF) '%s := %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2)

# 1. Variable name.
# 2. Value.
gen_make_var_list = \
	$(PRINTF) '%s := $(foreach ,$2,\\\n\t%s)\n\n' \
		$(call sh_quote,$1) \
		$(foreach v,$2,$(call sh_quote,$v))

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
	$(PRINTF) '%s : %s := %s\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(call sh_quote,$3)
# 1. Target.
# 2. Funtion name.
# 3. Function body.
gen_make_tsfn = \
	$(PRINTF) '%s : %s = %s\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \
		$(call sh_quote,$3)


# 1. Variable name.
# 2. Value.
gen_add_var = \
	$(PRINTF) '%s += %s\n\n' \
		$(call sh_quote,$1) \
		$(call sh_quote,$2) \

# 1. Target.
# 2. Variable name.
# 3. Value.
gen_add_tsvar = \
	$(PRINTF) '%s : %s += %s\n\n' \
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

# 1. Makefile
# 2. Whether to use silent '-include'.
gen_make_include = \
	$(PRINTF) '%sinclude %s\n\n' \
		$(call sh_quote,$(if $(value 2),-)) \
		$(call sh_quote,$1)

# Working with these lists...

build_modules := \
	$(call get,$(build_model),modules)
build_sources := \
	$(foreach m,$(build_modules), \
		$(addprefix $m,$(call get,$m,sources)))

# 1. Annotation target
# 2. Annotation option
annotation_value = $(call get,$(call invoke,$1,getAnnotationValuesOfOption,$2),value)

#
# Global artifacts.
#

@build_image := \
	$(build_model:%=build-image-rmk/%)

@build_include_mk := \
	$(build_model:%=build-include-mk/%)

@build_initfs := \
	$(build_model:%=build-initfs-rmk/%)

@build_include_install := \
	$(build_model:%=build-include_install-rmk/%)

@build_all = \
	$(@build_include_install) \
	$(@build_image) \
	$(@build_include_mk) \
	$(@build_initfs)

all .PHONY : $(@build_all)

$(@build_all) : a_modules = $(suffix $(@module_ar_rmk))
$(@build_all) : o_modules = $(suffix $(@module_ld_rmk))

build_image_rmk_mk_pat     = $(MKGEN_DIR)/%.rule.mk
build_image_rmk_target_pat = $1

$(@build_image) : @file    = $(patsubst %,$(build_image_rmk_mk_pat),image)
$(@build_image) : mk_file  = \
		$(patsubst %,$(value build_image_rmk_mk_pat),image)
$(@build_image) : target_file = \
		$(patsubst %,$(value build_image_rmk_target_pat),image)

my_bld       := $(call mybuild_resolve_or_die,mybuild.lang.Build)
my_bld_stage := $(call mybuild_resolve_or_die,mybuild.lang.Build.stage)
my_autocmd   := $(call mybuild_resolve_or_die,mybuild.lang.AutoCmd)
my_autocmd_preserve_locals := $(call mybuild_resolve_or_die,mybuild.lang.AutoCmd.preserve_locals)
my_postbuild_script := $(call mybuild_resolve_or_die,mybuild.lang.Postbuild.script)

# Return modules of specified stage
# 1. Stage to filter
# 2. Modules list
filter_stage_modules = \
	$(foreach m,$2, \
	       $(if $(call eq,$1,$(or $(call annotation_value,$(call get,$m,allTypes),$(my_bld_stage)),$(and $(strip $(call invoke,$(call get,$m,allTypes),getAnnotationsOfType,$(my_bld))),2),1)), \
			$m))

__gen_stage = \
	$(call gen_make_var_list,__image_ld_scripts$1,$(patsubst %,$(value source_cpp_rmk_out_pat), \
		$(call source_base,$(@source_cpp_rmk)))); \
	$(call gen_make_var_list,__image_ld_objs$1,$(patsubst %,$(value module_ld_rmk_out_pat), \
		$(call module_path,$(call filter_stage_modules,$1,$(o_modules))))); \
	$(call gen_make_var_list,__image_ld_libs$1,$(patsubst %,$(value module_ar_rmk_a_pat), \
		$(call module_path,$(call filter_stage_modules,$1,$(a_modules)))))

$(@build_image) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,__image_mk_file,$(mk_file)); \
		$(call __gen_stage,1); \
		$(call __gen_stage,2))

$(@build_include_mk) : @file = $(MKGEN_DIR)/include.mk
$(@build_include_mk) : image_rmk = \
		$(patsubst %,$(value build_image_rmk_mk_pat),image)
$(@build_include_mk) : source_rmk = \
		$(patsubst %,$(value source_rmk_mk_pat), \
			$(call source_file,$(@source_rmk)))
$(@build_include_mk) : module_ar_rmk = \
		$(patsubst %,$(value module_ar_rmk_mk_pat), \
			$(call module_path,$(@module_ar_rmk)))
$(@build_include_mk) : module_ld_rmk = \
		$(patsubst %,$(value module_ld_rmk_mk_pat), \
			$(call module_path,$(@module_ld_rmk)))
$(@build_include_mk) : module_extbld_rmk = \
		$(patsubst %,$(value module_extbld_rmk_mk_pat), \
			$(call module_path,$(@module_extbld_rmk)))
$(@build_include_mk) : initfs_rmk = \
		$(patsubst %,$(value build_initfs_rmk_mk_pat),$(build_initfs))
###################
#Include install
$(@build_include_mk) : source_include_install_rmk = \
		$(patsubst %,$(value build_include_install_rmk_mk_pat), \
			$(call source_file,$(@source_include_install)))

#######################

$(@build_include_mk) : cmd_file = $(@file).cmd
$(@build_include_mk) :
	@$(call cmd_assemble,$(cmd_file), \
		$(gen_banner); \
		$(call gen_make_var,__include_image,$(image_rmk)); \
		$(call gen_make_var,__include_initfs,$(initfs_rmk)); \
		$(call gen_make_var_list,__include, \
			$(source_rmk) $(module_extbld_rmk) \
			$(module_ar_rmk) $(module_ld_rmk)); \
		$(call gen_make_var_list,__include_incinst, \
			$(source_include_install_rmk)); \
		$(call gen_make_var_list,__module_list, \
			$(call get,$(call get,$(build_modules),type),qualifiedName)))
	@$(call cmd_notouch_stdout,$(@file), \
		sh $(cmd_file))

build_initfs := initfs

$(@build_initfs) : initfs = $(build_initfs)

build_initfs_rmk_mk_pat     = $(MKGEN_DIR)/%.rule.mk
build_initfs_rmk_target_pat = $(ROOTFS_IMAGE)

$(@build_initfs) : @file    = $(initfs:%=$(build_initfs_rmk_mk_pat))
$(@build_initfs) : mk_file  = \
		$(patsubst %,$(value build_initfs_rmk_mk_pat),$$(build_initfs))
$(@build_initfs) : target_file = \
		$(patsubst %,$(value build_initfs_rmk_target_pat),$$(build_initfs))

$(@build_initfs) : cpio_files = \
		$(call source_initfs_cp_out,$(@source_initfs_cp_rmk))

$(@build_initfs) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,build_initfs,$(initfs)); \
		$(call gen_make_dep,$(target_file),$$$$(initfs_prerequisites)); \
		$(call gen_make_tsvar,$(target_file),mk_file,$(mk_file)); \
		$(call gen_make_tsvar_list,$(target_file),cpio_files,$(cpio_files)); \
		$(call gen_make_var_list,__cpio_files,$(cpio_files)))

####################
#Include Install
build_include_install := include_install

$(@build_include_install) : include_install = $(build_include_install)

build_include_install_rmk_mk_pat     = $(MKGEN_DIR)/%.rule.mk
build_include_install_rmk_target_pat = $(INCLUDE_INSTALL)

$(@build_include_install) : @file    = $(include_install:%=$(build_include_install_rmk_mk_pat))
$(@build_include_install) : mk_file  = \
		$(patsubst %,$(value build_include_install_rmk_mk_pat),$$(build_include_install))
$(@build_include_install) : target_file = \
		$(patsubst %,$(value build_include_install_rmk_target_pat),$$(build_include_install))
$(@build_include_install) : include_install_files = \
		$(call source_include_install_out,$(@source_include_install))

$(@build_include_install) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,build_include_install,$(include_install)); \
		$(call gen_make_dep,$(target_file),$$$$(include_install_prerequisites)); \
		$(call gen_make_tsvar,$(target_file),mk_file,$(mk_file)); \
		$(call gen_make_tsvar_list,$(target_file),include_install_files,$(include_install_files)))
###########################

##################
#
# Per-module artifacts.
#

module_type_fqn = $(call get,$1,qualifiedName)
module_fqn  = $(call module_type_fqn,$(call get,$1,type))
module_type_path = $(subst .,/,$(module_type_fqn))
module_path = $(subst .,/,$(call module_type_fqn,$(call get,$1,type)))
module_id   = $(subst .,__,$(module_fqn))

my_add_prefix := $(call mybuild_resolve_or_die,mybuild.lang.AddPrefix.value)

source_member = $(call invoke,$1,eContainer)
source_annotation_values = \
	$(call invoke,$(source_member),getAnnotationValuesOfOption,$2)
source_annotations = \
	$(call invoke,$(source_member),getAnnotationsOfType,$2)

__source_file_mod = $(strip \
	$(subst ^MOD_PATH,$(call module_type_path,$2), \
		$(if $(findstring ^BUILD,$3), \
			../..$(subst ^BUILD,,$3), \
			$(patsubst %$(call get,$1,fileName),%, \
				$(call get,$1,fileFullName))$3)))
__source_file_wprefix = $(subst $(\s),, \
	$(foreach p, \
		$(call annotation_value,$(source_member),$(my_add_prefix)), \
		$(strip $p)/))$(call get,$1,fileName)
source_file = $(strip \
	$(foreach 1,$1, \
		$(call __source_file_mod,$1,$(call invoke,$(source_member),eContainer),$(__source_file_wprefix))))
# source_file = \
# 	$(warning $@ [$(__source_file)])$(__source_file)

source_base = $(basename $(source_file))

source_o_pats   := %.o
source_a_pats   := %.a
source_cc_pats  := %.S %.c %.cpp %.cxx %.C %.cc
source_mk_pats  := %.mk
source_cpp_pats := %.lds.S

module_o_source_files = \
	$(filter $(source_cc_pats) $(source_o_pats),$(filter-out $(source_cpp_pats), \
		$(call source_file,$(call get,$1,sources))))

module_a_source_files = \
	$(filter $(source_a_pats),$(call source_file,$(call get,$1,sources)))

filter_with_sources = \
	$(foreach 1,$1,$(if $(module_o_source_files)$(module_a_source_files),$1))

static_modules    := $(call filter_static_modules,$(build_modules))
nonstatic_modules := $(filter-out $(static_modules), $(build_modules))
static_modules_type    := $(foreach i,$(static_modules),$(call get,$i,type) )
nonstatic_modules_type := $(foreach i,$(nonstatic_modules),$(call get,$i,type) )

@module_ld_rmk := \
	$(patsubst %,module-ld-rmk/%, \
		$(call filter_with_sources,$(nonstatic_modules)))

@module_ar_rmk := \
	$(patsubst %,module-ar-rmk/%, \
		$(call filter_with_sources,$(static_modules)))

my_app := $(call mybuild_resolve_or_die,mybuild.lang.App)

@module_h := \
	$(foreach m,$(build_modules), \
		$(foreach t,$(call get,$m,allTypes), \
			module-header-h/$t$m module-config-h/$t$m))

my_bld_script := $(call mybuild_resolve_or_die,mybuild.lang.Build.script)

my_bld_dep_value := $(call mybuild_resolve_or_die,mybuild.lang.BuildDepends.value)

my_bld_artpath_cppflags_before := $(call mybuild_resolve_or_die,mybuild.lang.BuildArtifactPath.cppflags_before)
my_bld_artpath_cppflags := $(call mybuild_resolve_or_die,mybuild.lang.BuildArtifactPath.cppflags)
my_bld_artpath_ldflags := $(call mybuild_resolve_or_die,mybuild.lang.BuildArtifactPath.ldflags)

@module_extbld_rmk := \
	$(foreach m,$(build_modules), \
		$(patsubst %,module-extbld-rmk/%$m, \
			$(call invoke,$(call get,$m,allTypes),getAnnotationValuesOfOption,$(my_bld_script))))
@module_all = \
	$(@module_h) \
	$(@module_ld_rmk) \
	$(@module_ar_rmk) \
	$(@module_extbld_rmk)

all .PHONY : $(@module_all)

module_my_file = \
	$(call get,$(call get,$(call get,$1,type),eResource),fileName)

$(@module_all) : fqn   = $(call module_fqn,$@)
$(@module_all) : path  = $(call module_path,$@)
$(@module_all) : my_file = $(call module_my_file,$@)


module_ar_rmk_mk_pat = $(MKGEN_DIR)/module/%.ar_rule.mk
module_ar_rmk_a_pat  = $(OBJ_DIR)/module/%.a

module_ld_rmk_mk_pat = $(MKGEN_DIR)/module/%.ld_rule.mk
module_ld_rmk_out_pat  = $(OBJ_DIR)/module/%.o

$(@module_ld_rmk) : kind := ld
$(@module_ar_rmk) : kind := ar

$(@module_ld_rmk) $(@module_ar_rmk) : @file   = \
		$(path:%=$(module_$(kind)_rmk_mk_pat))
$(@module_ld_rmk) $(@module_ar_rmk) : mk_file = \
		$(patsubst %,$(value module_$(kind)_rmk_mk_pat),$$(module_path))

$(@module_ld_rmk) : out = \
		$(patsubst %,$(value module_ld_rmk_out_pat),$$(module_path))
$(@module_ar_rmk) : out = \
		$(patsubst %,$(value module_ar_rmk_a_pat),$$(module_path))

$(@module_ld_rmk) $(@module_ar_rmk) : o_files = \
		$(patsubst %,$(value source_o_rmk_out_pat), \
			$(basename $(call module_o_source_files,$@)))

$(@module_ld_rmk) $(@module_ar_rmk) : a_files = \
		$(patsubst %,$(value source_a_rmk_out_pat), \
			$(basename $(call module_a_source_files,$@)))

$(@module_ld_rmk) $(@module_ar_rmk) : id_ = \
		$(call module_id,$@)

$(@module_ld_rmk) $(@module_ar_rmk) : is_app = \
		$(if $(strip $(call invoke, \
				$(call get,$@,allTypes),getAnnotationsOfType,$(my_app))),1)

my_linker_section_text   := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.text)
my_linker_section_rodata := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.rodata)
my_linker_section_data   := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.data)
my_linker_section_bss    := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.bss)

$(@module_ld_rmk) $(@module_ar_rmk) : linker_section_text = \
		$(strip $(call annotation_value, \
			$(call get,$@,allTypes),$(my_linker_section_text)))

$(@module_ld_rmk) $(@module_ar_rmk) : linker_section_rodata = \
		$(strip $(call annotation_value, \
			$(call get,$@,allTypes),$(my_linker_section_rodata)))

$(@module_ld_rmk) $(@module_ar_rmk) : linker_section_data = \
		$(strip $(call annotation_value, \
			$(call get,$@,allTypes),$(my_linker_section_data)))

$(@module_ld_rmk) $(@module_ar_rmk) : linker_section_bss = \
		$(strip $(call annotation_value, \
			$(call get,$@,allTypes),$(my_linker_section_bss)))

build_deps = $(call annotation_value,$1,$(my_bld_dep_value))

# Maps moduleType to that one, which instance is in build. For example, if 'api' extended
# by 'impl1' and 'impl2', and 'impl2' is in build, calling this with 'api' moduleType will
# return moduleType of 'impl2'.
# 1. moduleType
build_module_type_substitude = \
	$(call get,$(call module_build_fqn2inst,$(call get,$1,qualifiedName)),type)

__build_deps_recurse = $1 $(call build_deps_all,$1)

build_deps_all = \
	$(sort $(foreach d,$(call build_deps,$1), \
		$(call __build_deps_recurse,$(call build_module_type_substitude,$d))))

cond_add=$(if $(strip $1),$1$2)

$(@module_ld_rmk) $(@module_ar_rmk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,module_path,$(path)); \
		$(call gen_make_dep,$(out),$$$$($(kind)_prerequisites)); \
		$(call gen_make_tsvar,$(out),module_id,$(id_)); \
		$(call gen_make_tsvar,$(out),is_app,$(is_app)); \
		$(call gen_make_tsvar,$(out),linker_section_text,$(linker_section_text)); \
		$(call gen_make_tsvar,$(out),linker_section_rodata,$(linker_section_rodata)); \
		$(call gen_make_tsvar,$(out),linker_section_data,$(linker_section_data)); \
		$(call gen_make_tsvar,$(out),linker_section_bss,$(linker_section_bss)); \
		$(call gen_make_tsvar,$(out),mod_path,$(path)); \
		$(call gen_make_tsvar,$(out),my_file,$(my_file)); \
		$(call gen_make_tsvar,$(out),mk_file,$(mk_file)); \
		$(call gen_make_tsfn,$(out),mod_postbuild, \
			$(if $(strip $(call invoke, \
				$(call get,$@,allTypes),getAnnotationsOfType,$(my_autocmd))),\
				$(call mod_autocmd_postbuild,$(call annotation_value,$(call get,$@,allTypes),$(my_autocmd_preserve_locals)));) \
			$(call cond_add,$(call annotation_value,$(call get,$@,allTypes),$(my_postbuild_script)),;)); \
		$(call gen_make_tsvar_list,$(out),o_files,$(o_files)); \
		$(call gen_make_tsvar_list,$(out),a_files,$(a_files)))


module_header_h_pat = $(SRCGEN_DIR)/include/module/%.h
module_config_h_pat = $(SRCGEN_DIR)/include/config/%.h

$(@module_h) : kind = $(patsubst module-%-h,%,$(@D))

$(@module_h) : @file = $(type_path:%=$(module_$(kind)_h_pat))
$(@module_h) : type = $(basename $@)
$(@module_h) : type_fqn = $(call get,$(type),qualifiedName)
$(@module_h) : type_path = $(subst .,/,$(type_fqn))

$(@module_h) : content = $(call __module_$(kind)_h_template,$@,$(type))

$(@module_h) : printf_escape = \
		$(subst $(\t),\t,$(subst $(\n),\n,$(subst \,\\,$1)))

$(@module_h) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(PRINTF) '%b' $(call sh_quote,$(call printf_escape,$(content))))


module_extbld_rmk_mk_pat     = $(MKGEN_DIR)/%.extbld_rule.mk
module_extbld_rmk_target_pat = $(MKGEN_DIR)/%.__extbld-target

$(@module_extbld_rmk) : @file   = $(path:%=$(module_extbld_rmk_mk_pat))
$(@module_extbld_rmk) : mk_file = $(patsubst %,$(value module_extbld_rmk_mk_pat),$$(module_path))
$(@module_extbld_rmk) : target = $(patsubst %,$(value module_extbld_rmk_target_pat),$$(module_path))
$(@module_extbld_rmk) : script = $(call get,$(basename $@),value)
$(@module_extbld_rmk) : stage = $(or $(strip $(call annotation_value,$(call get,$@,allTypes),$(my_bld_stage))),2)
$(@module_extbld_rmk) : __build_deps = $(call build_deps, $(call get,$@,allTypes))
$(@module_extbld_rmk) : __build_deps_all = $(call build_deps_all, $(call get,$@,allTypes))
$(@module_extbld_rmk) : this_build_deps = $(patsubst %,$(value module_extbld_rmk_target_pat),$(call module_type_path,$(__build_deps)))
$(@module_extbld_rmk) : __build_deps_artpath_cppflags_before = $(call annotation_value,$(__build_deps_all),$(my_bld_artpath_cppflags_before))
$(@module_extbld_rmk) : __build_deps_artpath_cppflags_after  = $(call annotation_value,$(__build_deps_all),$(my_bld_artpath_cppflags))
$(@module_extbld_rmk) : __build_deps_artpath_ldflags = $(call annotation_value,$(__build_deps),$(my_bld_artpath_ldflags))

$(@module_extbld_rmk) : kind := extbld

$(@module_extbld_rmk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,module_path,$(path)); \
		$(call gen_make_dep,__extbld-$(stage),$(target)); \
		$(call gen_make_dep,$(target),$$$$($(kind)_prerequisites)); \
		$(call gen_make_tsvar,$(target),mod_path,$(path)); \
		$(call gen_make_tsvar,$(target),my_file,$(my_file)); \
		$(call gen_make_tsvar,$(target),mk_file,$(mk_file)); \
		$(call gen_make_tsvar,$(target),mk_file,$(mk_file)); \
		$(call gen_add_tsvar,$(target),BUILD_DEPS_CPPFLAGS_BEFORE,$(__build_deps_artpath_cppflags_before)); \
		$(call gen_add_tsvar,$(target),BUILD_DEPS_CPPFLAGS_AFTER,$(__build_deps_artpath_cppflags_after)); \
		$(call gen_add_tsvar,$(target),BUILD_DEPS_LDFLAGS,$(__build_deps_artpath_ldflags)); \
		$(foreach d,$(this_build_deps),$(call gen_make_rule,$(dir $d)%,,@true); ) \
		$(call gen_make_rule,$(target), | $(this_build_deps),$(script)))

#
# Per-source artifacts.
#

my_gen_script := $(call mybuild_resolve_or_die,mybuild.lang.Generated.script)

@source_gen := \
	$(foreach s,$(build_sources), \
		$(patsubst %,source-gen/%$s, \
			$(call source_annotation_values,$s,$(my_gen_script))))

#####################
#Include Install
my_include_install := $(call mybuild_resolve_or_die,mybuild.lang.IncludeExport)
my_include_install_dir := $(call mybuild_resolve_or_die,mybuild.lang.IncludeExport.path)
my_include_install_target_name := $(call mybuild_resolve_or_die,mybuild.lang.IncludeExport.target_name)

@source_include_install := \
	$(foreach s,$(build_sources), \
		$(patsubst %,source-include-install/%$s, \
			$(call source_annotations,$s,$(my_include_install))))
#####################

my_initfs := $(call mybuild_resolve_or_die,mybuild.lang.InitFS)
my_initfs_target_dir := $(call mybuild_resolve_or_die,mybuild.lang.InitFS.target_dir)
my_initfs_target_name := $(call mybuild_resolve_or_die,mybuild.lang.InitFS.target_name)
my_initfs_chmod := $(call mybuild_resolve_or_die,mybuild.lang.InitFS.chmod)
my_initfs_chown := $(call mybuild_resolve_or_die,mybuild.lang.InitFS.chown)
my_initfs_xattr := $(call mybuild_resolve_or_die,mybuild.lang.InitFS.xattr)

@source_initfs_cp_rmk := \
	$(foreach s,$(build_sources), \
		$(if $(call source_annotations,$s,$(my_initfs)), \
			source-initfs-cp-rmk/$s))

@source_rmk := \
	$(foreach s,$(build_sources), \
		$(foreach f,$(call get,$s,fileName),$(or \
			$(and $(filter $(source_cpp_pats),$f),source-cpp-rmk/$s), \
			$(and $(filter $(source_cc_pats),$f), source-cc-rmk/$s), \
			$(and $(filter $(source_mk_pats),$f), source-mk-rmk/$s), \
			$(and $(filter $(source_a_pats),$f),  source-a-rmk/$s), \
			$(and $(filter $(source_o_pats),$f),  source-o-rmk/$s))))

@source_o_rmk   := $(filter source-o-rmk/%,$(@source_rmk))
@source_a_rmk   := $(filter source-a-rmk/%,$(@source_rmk))
@source_cc_rmk  := $(filter source-cc-rmk/%,$(@source_rmk))
@source_mk_rmk  := $(filter source-mk-rmk/%,$(@source_rmk))
@source_cpp_rmk := $(filter source-cpp-rmk/%,$(@source_rmk))

$(@source_o_rmk)   : kind := o
$(@source_a_rmk)   : kind := a
$(@source_cc_rmk)  : kind := cc
$(@source_mk_rmk)  : kind := mk
$(@source_cpp_rmk) : kind := cpp

@source_rmk += \
	$(@source_include_install) \
	$(@source_initfs_cp_rmk)

# task/???.module.source
@source_all = \
	$(@source_gen) \
	$(@source_rmk)

all .PHONY : $(@source_all)

$(@source_all) : module = $(basename $@)
$(@source_all) : mod_path = $(call module_path,$(module))
$(@source_all) : my_file = $(call module_my_file,$(module))

$(@source_all) : file = $(call source_file,$@)
$(@source_all) : base = $(call source_base,$@)

$(@source_all) : values_of = $(call source_annotation_values,$@,$1)

my_rule_script  = $(call mybuild_resolve_or_die,mybuild.lang.Rule.script)
my_rule_prereqs = $(call mybuild_resolve_or_die,mybuild.lang.Rule.prerequisites)

$(@source_all) : script  = $(call get,$(call values_of,$(my_rule_script)),value)
$(@source_all) : prereqs = $(call get,$(call values_of,$(my_rule_prereqs)),value)

$(@source_o_rmk) $(@source_a_rmk) : script  = $(or \
			$(call get,$(call values_of,$(my_rule_script)),value), \
			@true)

my_incpath_before_val := \
		$(call mybuild_resolve_or_die,mybuild.lang.IncludePathBefore.value)
my_incpath_val  := $(call mybuild_resolve_or_die,mybuild.lang.IncludePath.value)
my_defmacro_val := $(call mybuild_resolve_or_die,mybuild.lang.DefineMacro.value)
my_cflags_val := $(call mybuild_resolve_or_die,mybuild.lang.Cflags.value)
my_instrument_val := \
		$(call mybuild_resolve_or_die,mybuild.lang.InstrumentProfiling.value)

$(@source_rmk) : includes_before = $(call values_of,$(my_incpath_before_val))
$(@source_rmk) : includes = $(call values_of,$(my_incpath_val))
$(@source_rmk) : defines  = $(call values_of,$(my_defmacro_val))
$(@source_rmk) : additional_cflags  = $(call values_of,$(my_cflags_val))
$(@source_rmk) : instrument = $(call values_of,$(my_instrument_val))


$(@source_rmk) : do_flags = $(foreach f,$2,$1$(call sh_quote,$(call get,$f,value)))
$(@source_rmk) : check_profiling = $(if $(filter true, $(call get,$1,value)), -finstrument-functions, )
$(@source_rmk) : flags_before = $(call trim, \
			$(call do_flags,-I,$(includes_before)) \
			$(call annotation_value,$(call build_deps_all,$(call get,$(module),allTypes)),$(my_bld_artpath_cppflags_before)))
$(@source_rmk) : flags = $(call trim, \
			$(call do_flags,-I,$(includes)) \
			$(call annotation_value,$(call build_deps_all,$(call get,$(module),allTypes)),$(my_bld_artpath_cppflags)) \
			$(call do_flags,-D,$(defines)) \
			-include $(patsubst %,$(value module_config_h_pat), \
						$(mod_path)) \
			-D__EMBUILD_MOD__=$(call module_id,$(module)) \
			$(call check_profiling,$(instrument)) \
			$(call do_flags,,$(additional_cflags)))

source_rmk_mk_pat   = $(MKGEN_DIR)/%.rule.mk

$(@source_rmk) : @file   = $(file:%=$(source_rmk_mk_pat))
$(@source_rmk) : mk_file = $(patsubst %,$(value source_rmk_mk_pat),$$(source_file))

source_o_rmk_out_pat   = $(OBJ_DIR)/%.o
source_a_rmk_out_pat   = $(OBJ_DIR)/%.a
source_cc_rmk_out_pat  = $(OBJ_DIR)/%.o
source_cpp_rmk_out_pat = $(OBJ_DIR)/%# foo.lds.S -> foo.lds

$(@source_rmk)  : out = $(patsubst %,$(value source_$(kind)_rmk_out_pat),$$(source_base))

$(@source_cpp_rmk) $(@source_cc_rmk) $(@source_o_rmk) $(@source_a_rmk):
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_var,mod_path,$(mod_path)); \
		$(call gen_make_var,source_file,$(file)); \
		$(call gen_make_var,source_base,$$(basename $$(source_file))); \
		$(call gen_make_dep,$(out),$$$$($(kind)_prerequisites)); \
		$(call gen_make_tsvar,$(out),mod_path,$(mod_path)); \
		$(call gen_make_tsvar,$(out),my_file,$(my_file)); \
		$(call gen_make_tsvar,$(out),mk_file,$(mk_file)); \
		$(call gen_make_tsvar,$(out),flags_before,$(flags_before)); \
		$(call gen_make_tsvar,$(out),flags,$(flags)); \
		$(call gen_make_rule,$(out),$(prereqs),$(script)); \
		$(call gen_make_include,$$(OBJ_DIR)/$$(source_base).d,silent))

$(@source_mk_rmk):
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_include,$(file)))

source_initfs_cp_target_dir = $(call get,$(call source_annotation_values,$s,$(my_initfs_target_dir)),value)/
source_initfs_cp_target_name=$(or $(strip \
	$(call get,$(call source_annotation_values,$s,$(my_initfs_target_name)),value)),$(call get,$s,fileName))
source_initfs_cp_out = $(addprefix $$(ROOTFS_DIR)/, \
		$(foreach s,$1,$(source_initfs_cp_target_dir:/%=%)$(source_initfs_cp_target_name)))

$(@source_initfs_cp_rmk) : out = $(call source_initfs_cp_out,$@)
$(@source_initfs_cp_rmk) : src_file = $(file)
$(@source_initfs_cp_rmk) : mk_file = $(patsubst %,$(value source_rmk_mk_pat),$(file))
$(@source_initfs_cp_rmk) : kind := initfs_cp
$(@source_initfs_cp_rmk) : str_of = \
		$(call sh_quote,$(call get,$(call values_of,$1),value))
$(@source_initfs_cp_rmk) : chmod = $(call str_of,$(my_initfs_chmod))
$(@source_initfs_cp_rmk) : chown = $(call str_of,$(my_initfs_chown))
$(@source_initfs_cp_rmk) : xattr = $(call str_of,$(my_initfs_xattr))

$(@source_initfs_cp_rmk) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(gen_banner); \
		$(call gen_make_dep,$(out),$$$$($(kind)_prerequisites)); \
		$(call gen_make_tsvar,$(out),src_file,$(src_file)); \
		$(call gen_make_tsvar,$(out),chmod,$(chmod)); \
		$(call gen_make_tsvar,$(out),chown,$(chown)); \
		$(call gen_make_tsvar_list,$(out),xattr,$(xattr)); \
		$(call gen_make_tsvar,$(out),mod_path,$(mod_path)); \
		$(call gen_make_tsvar,$(out),my_file,$(my_file)); \
		$(call gen_make_tsvar,$(out),mk_file,$(mk_file)))

$(@source_gen) : @file = $(SRCGEN_DIR)/$(file)
$(@source_gen) : gen_string = $(basename $(basename $@))
$(@source_gen) : script = $(call expand,$(call get,$(gen_string),value))

$(@source_gen) :
# Plus sign is for case when generator script is also Make-based.
# Avoids 'jobserver unavailable' warning for parallel builds.
	+@$(call cmd_notouch_stdout,$(@file), \
		$(script))

###########################
#Include Install
source_include_install_dir=$(call get,$(call source_annotation_values,$s,$(my_include_install_dir)),value)
source_include_install_target_name=$(or $(strip \
	$(call get,$(call source_annotation_values,$s,$(my_include_install_target_name)),value)),$(call get,$s,fileName))
source_include_install_out = $(addprefix $$(INCUDE_INSTALL_DIR)/, \
	       $(foreach s,$1,$(source_include_install_dir)/$(source_include_install_target_name)))

$(@source_include_install) : out = $(call source_include_install_out,$@)
$(@source_include_install) : src_file = $(file)
$(@source_include_install) : mk_file = $(patsubst %,$(value source_rmk_mk_pat),$(file))

$(@source_include_install) :
	@$(call cmd_notouch_stdout,$(@file), \
		$(call gen_make_dep,$(out),$$$$(include_install_prerequisites)); \
		$(call gen_make_tsvar,$(out),src_file,$(src_file)); \
		$(call gen_make_tsvar,$(out),my_file,$(my_file)); \
		$(call gen_make_tsvar,$(out),mk_file,$(mk_file)))

#############################

ifdef GEN_DIST

%/. :
	@$(MKDIR) $*

.SECONDEXPANSION :

@source_dist := \
	$(addprefix source-dist/,$(notdir $(build_sources)))

all .PHONY : $(@source_dist)

$(@source_dist) : file = $(patsubst ../../%,../%,$(call source_file,$@))
$(@source_dist) : @file = $(DIST_BASE_DIR)/$(file)
$(@source_dist) : @dir = $(patsubst %/,%,$(dir $(@file)))
$(@source_dist) : | $$(@dir)/.
$(@source_dist) :
	@if [ -e $(file) ]; then cp -Trf $(file) $(@file); fi

@dist_makefile := dist-makefile-/$(DIST_BASE_DIR)/../Makefile

@dist_cpfiles := $(addprefix dist-cpfile-/$(DIST_BASE_DIR)/, \
	mk/core/common.mk \
	mk/core/string.mk \
	$(wildcard mk/extbld/*) \
	mk/script/application_template.c \
	mk/script/lds-apps.mk \
	mk/script/nm2c.awk \
	mk/script/qt-plugin.mk \
	mk/script/script-common.mk \
	mk/arhelper.mk \
	mk/build-dist.mk \
	mk/extbld.mk \
	mk/flags.mk \
	mk/image.lds.S \
	mk/image2.mk \
	mk/image3.mk \
	mk/image_lib.mk \
	mk/main-stripping.mk \
	mk/main-dist.mk \
	mk/main-stripping.sh \
	mk/phymem_cc_addon.tmpl.c \
	mk/variables.mk \
	mk/version.mk)

@dist_cpfiles += $(addprefix dist-cpfile-/$(DIST_BASE_DIR)/, \
	$(SRC_DIR)/arch/$(ARCH)/embox.lds.S)

__source_dirs := $(sort $(dir $(call source_file,$(build_sources))))
@dist_cpfiles += $(addprefix dist-cpfile-/$(DIST_BASE_DIR)/, \
	$(wildcard $(foreach e,*.h include/*.h include/*/*.h *.inc Makefile *.txt *.patch *.diff, \
		$(addsuffix $e,$(__source_dirs)))))

include mk/flags.mk  # INCLUDES_FROM_FLAGS

@dist_includes := $(addprefix dist-includes-/,$(sort \
	$(call filter-patsubst,$(abspath $(ROOT_DIR))/%,$(DIST_BASE_DIR)/%, \
		$(filter-out $(abspath \
				$(CONF_DIR) $(CONF_DIR)/% \
				$(EXTERNAL_BUILD_DIR) $(EXTERNAL_BUILD_DIR)/%),$(abspath \
			$(call expand,$(call get, \
				$(sort $(call source_annotation_values,$(build_sources), \
					$(my_incpath_val) $(my_incpath_before_val))),value)) \
			$(INCLUDES_FROM_FLAGS))))))

# remove nested directories
@dist_includes := \
	$(filter-out $(addsuffix /%,$(@dist_includes)),$(@dist_includes))

@dist_conf := $(addprefix dist-conf-/$(DIST_BASE_DIR)/conf/, \
	rootfs \
	start_script.inc)

@dist_all := \
	$(@dist_makefile) \
	$(@dist_cpfiles) \
	$(@dist_includes) \
	$(@dist_conf)


all .PHONY : $(@dist_all)
$(@dist_makefile) : dist-makefile-/% : | $$(*D)/.
$(@dist_makefile) : @file = $(@:dist-makefile-/%=%)
$(@dist_makefile) : file = $(@:dist-makefile-/$(DIST_BASE_DIR)/../%=$(ROOT_DIR)/%)
$(@dist_makefile) :
	@sed 's/\$$(dir \$$(lastword \$$(MAKEFILE_LIST)))/\$$(dir \$$(lastword \$$(MAKEFILE_LIST)))\/base/g' < $(file) > $(@file)

$(@dist_cpfiles) : dist-cpfile-/% : | $$(*D)/.
$(@dist_cpfiles) : @file = $(foreach f,$(@:dist-cpfile-/%=%),$(dir $f)$(subst -dist.,.,$(notdir $f)))
$(@dist_cpfiles) : file = $(@:dist-cpfile-/$(DIST_BASE_DIR)/%=$(ROOT_DIR)/%)
$(@dist_cpfiles) :
	@cp -Trf $(file) $(@file)


$(@dist_includes) : dist-includes-/% : | $$(*D)/.
$(@dist_includes) : @file = $(@:dist-includes-/%=%)
$(@dist_includes) : file = $(@file:$(DIST_BASE_DIR)/%=$(ROOT_DIR)/%)

$(@dist_conf) : dist-conf-/% : | $$(*D)/.
$(@dist_conf) : @file = $(@:dist-conf-/%=%)
$(@dist_conf) : file = $(@file:$(DIST_BASE_DIR)/conf/%=$(CONF_DIR)/%)

$(@dist_includes) $(@dist_conf) :
	@if [ -e $(file) ]; then cp -Trf $(file) $(@file); fi

endif # GEN_DIST
