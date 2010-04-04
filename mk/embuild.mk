#
# Author: Eldar Abusalimov
#

ifndef _embuild_mk_
_embuild_mk_ := 1

EMBUILD_DUMP_MK := $(CODEGEN_DIR)/embuild_cache.mk

#$(info Running EMBuild [$(MAKELEVEL)])

include $(MK_DIR)/traverse.mk
include $(MK_DIR)/util.mk

# Guard symbol. Use it only in contexts with $(dir) defined.
_ = EMBUILD/$(abspath $(dir))/

# Traverse always defines SELFDIR before entering sub-makefile.
dir = $(SELFDIR)

DIRS := $(call TRAVERSE,$(SRC_DIR),Makefile) \
  $(if $(PLATFORM),$(call TRAVERSE,$(PLATFORM_DIR),Makefile))

# LDFLAGS are common for the entire image.
# Collect per-directory $_LDFLAGS definitions.
__LDFLAGS = $(strip $(foreach dir,$(DIRS),$($_LDFLAGS)))

# Name check for packages and units.
invalid_symbols := \
  ` ~ ! @ $$ % ^ & * ( ) { } [ ] < > ; : ' " \ | / ? + = № ..
check_name = \
  $(if $(or \
        $(filter .%,$(1)), \
        $(filter %.,$(1)), \
        $(strip $(foreach s,$(invalid_symbols),$(findstring $s,$(1)))) \
      ), \
    $(info $(error_str) Invalid name:: $(1)) \
    $(error Invalid package/unit name) \
  ) \

# First of all deal with package info:
# define implicit directory packages, assign compilation flags.

dir_package_lookup = \
  $(if $(filter $(abspath $(ROOT_DIR))%,$1),$(foreach dir,$1, \
    $(eval $_PACKAGE ?= $(strip $(call $0,$(dir $(1:%/=%))))) \
    $(eval $_PACKAGE := $($_PACKAGE)) \
    $(eval $_PACKAGE := $(if $($_PACKAGE),$($_PACKAGE),generic)) \
    $(call check_name,$($_PACKAGE)) \
    $($_PACKAGE) \
  ))

# Get package list.
# This also provokes every $_PACKAGE variable to be explicitly defined.
__PACKAGES = $(info Listing packages) \
  $(sort $(foreach dir,$(DIRS),$(call dir_package_lookup,$(abspath $(dir)))))

package_symbol = $(if $(filter $($_PACKAGE),$(package)),$($_$1)) \
  $(if $(filter $(flavor $_$1),simple),$(eval $1-$(package)-(SIMPLE) := 1))

parent_package_symbol = \
  $(if $($1-$(package)-(SIMPLE)),,$(if $(findstring .,$(package)), \
    $(call __parent_package_symbol,$1,$(basename $(package))) \
  ))
__parent_package_symbol = \
  $(if $2, \
    $(if $(findstring .,$2),$(call $0,$1,$(basename $2))) \
  $($1-$2) \
)

define define_package_symbols_prepare
  CPPFLAGS-$(package) :=
  CFLAGS-$(package)   :=
endef

define define_package_symbols_per_directory
  CPPFLAGS-$(package) += $(call package_symbol,CPPFLAGS)
  CFLAGS-$(package)   += $(call package_symbol,CFLAGS)
endef

define define_package_symbols
  CPPFLAGS-$(package) := $(strip $(CPPFLAGS-$(package)) \
      $(call parent_package_symbol,CPPFLAGS))
  CFLAGS-$(package)   :=   $(strip $(CFLAGS-$(package)) \
        $(call parent_package_symbol,CFLAGS))
endef

__PACKAGES_PROCESS = $(info Processing packages) \
  $(foreach package,$(PACKAGES), \
    $(eval $(value define_package_symbols_prepare)) \
    $(foreach dir,$(DIRS), \
      $(eval $(value define_package_symbols_per_directory)) \
    ) \
    $(eval $(value define_package_symbols)) \
  )

# Canonical unit name is one with package prefix.
canonize_name = \
  $(foreach name,$1, \
    $(call check_name,$(name)) \
    $(if $(findstring .,$(name)), \
      $(name), \
      $($_PACKAGE).$(name) \
    ) \
  )

# Get list of all modules and libraries.
unit_def = \
  $(foreach unit,$1, \
    $(unit) $(eval UNIT_DEFINED-$(unit) := $(dir)) \
  )

mod_collect = \
  $(sort $(foreach dir,$(DIRS),$(call unit_def,$(call canonize_name,$($_$1)))))

# Mods that are always included to the resulting image
# (with their dependencies satisfied, of course).
__MODS_CORE = $(info Listing essentials) $(call mod_collect,MODS_CORE)
# Regular mods.
__MODS           = $(info Listing mods) $(call mod_collect,MODS)

lib_collect = $(sort $(foreach dir,$(DIRS),$(call unit_def,$($_$1))))

# Libraries.
__LIBS           = $(info Listing libs) $(call lib_collect,LIBS)

# Common units handling: source assignments and flags.

wildcard_srcs = $(wildcard $(1:%=$(dir)/%))

unit_package = $(basename $(unit))
unit_name = $(patsubst .%,%,$(suffix $(unit)))

unit_symbol = $($_$1-$(unit)) \
  $(if $(filter $($_PACKAGE),$(unit_package)),$($_$1-$(unit_name)))

#############################
#  $(eval WARNING_SRCS += $(abspath $(src))) \
#  $(eval WARNING_SRC-$(abspath $(src))-ctx_dir := $(dir)) \
#  $(eval WARNING_SRC-$(abspath $(src))-ctx_unit := $(unit)) \
#  $(eval WARNING_SRC-$(abspath $(src))-UNIT := $(UNIT-$(abspath $(src)))) \
#  $(eval WARNING_SRC-$(abspath $(src))-DEFINED := $(UNIT-$(abspath $(src))-DEFINED))

unit_srcs_check_warn = \
  $(info $(warning_str) \
    Repeated source assignment of $(src) to the same unit $(unit)) \
  $(info $(call warning_str_dir,$(UNIT-$(abspath $(src))-DEFINED)) \
    first defined here)

unit_srcs_check_error = \
  $(info $(error_str) \
    Attempting to reassign source $(src) to unit $(unit) \
      (already assigned to $(UNIT-$(abspath $(src))))) \
  $(info $(call error_str_dir,$(UNIT-$(abspath $(src))-DEFINED))\
    first defined here) \
  $(error Multiple source assignment) \

# Each source file should be assigned for a single unit. Prevent violation.
unit_srcs_check = \
  $(foreach src,$1, \
    $(if $(UNIT-$(abspath $(src))), \
      $(if $(filter $(UNIT-$(abspath $(src))),$(unit)), \
        $(unit_srcs_check_warn), \
        $(unit_srcs_check_error) \
      ), \
      $(eval UNIT-$(abspath $(src)) := $(unit)) \
      $(eval UNIT-$(abspath $(src))-DEFINED := $(dir)) \
      $(src) \
    ) \
  )

define define_unit_symbols_prepare
  SRCS-$(unit) :=
  CPPFLAGS-$(unit) :=
  CFLAGS-$(unit)   :=
endef

define define_unit_symbols_per_directory
  SRCS-$(unit) += \
    $(call unit_srcs_check,$(call wildcard_srcs,$(call unit_symbol,SRCS)))
  CPPFLAGS-$(unit) += $(call unit_symbol,CPPFLAGS)
  CFLAGS-$(unit)   += $(call unit_symbol,CFLAGS)
endef

define define_unit_symbols
  SRCS-$(unit) := $(strip $(SRCS-$(unit)))
  CPPFLAGS-$(unit) := $(strip $(CPPFLAGS-$(unit)))
  CFLAGS-$(unit)   := $(strip   $(CFLAGS-$(unit)))
endef

__UNITS_PROCESS = $(info Processing all units) \
  $(foreach unit,$(MODS) $(LIBS), \
    $(eval $(value define_unit_symbols_prepare)) \
    $(foreach dir,$(DIRS), \
      $(eval $(value define_unit_symbols_per_directory)) \
    ) \
    $(eval $(value define_unit_symbols)) \
  )

# Here goes mods specific functions (dependencies and so on).
unit = $(mod)

# The sub-graph of all module dependencies (either direct or indirect).
MOD_DEPS_DAG = $(sort $(call mod_deps_dag_walk,$1))
mod_deps_dag_walk = $(foreach mod,$1,$(call $0,$(DEPS-$(mod))) $(mod))

# User should list only existing mods in dependency list. Check it is true.
mod_deps_filter = \
  $(foreach dep, \
    $(if $(filter-out $(MODS),$1), \
      $(info $(warning_str) Undefined dependencies for mod $(mod):: \
        $(filter-out $(MODS),$1)) \
      $(filter $(MODS),$1), \
      $1 \
    ), \
    $(eval MOD-$(mod)-DEP-$(dep)-DEFINED := $(dir)) \
    $(dep) \
  )

define define_mod_symbols_prepare
  DEPS-$(mod) :=
endef

define define_mod_symbols_per_directory
  DEPS-$(mod) := $(DEPS-$(mod)) \
    $(call mod_deps_filter,$(call canonize_name,$(call unit_symbol,DEPS)))
endef

define define_mod_symbols
  DEPS-$(mod) := $(sort $(DEPS-$(mod)))
endef

# After dependency info has been collected for all mods we should check that
# the dependency graph is true DAG.
mod_detect_cycle_deps = \
  $(if $(filter $(mod),$1), \
    $(foreach pair, \
        $(call mod_dep_pairs,$2 $(lastword $3),$3 $(firstword $2)), \
      $(info $(call error_str_dir,$(MOD-$(subst /,-DEP-,$(pair))-DEFINED)) \
        Cyclic dependency definition here:: $(subst /, -> ,$(pair))) \
    ) \
    $(error Dependency cycle:: $(mod) $(strip $(3:%= -> %))), \
    $(foreach parent,$(if $(m),$(m),$(mod)),$(foreach m,$1,\
      $(call $0,$(DEPS-$(m)), \
        $2 $(parent),$3 $(m)) \
    )) \
  )
mod_dep_pairs = $(join $(1:%=%/),$(2))

# Define dependency info for each mod and then perform graph check.
__MODS_PROCESS = $(info Processing mod deps) \
  $(foreach mod,$(MODS), \
    $(eval $(value define_mod_symbols_prepare)) \
    $(foreach dir,$(DIRS), \
      $(eval $(value define_mod_symbols_per_directory)) \
    ) \
    $(eval $(value define_mod_symbols)) \
  ) \
  $(foreach mod,$(MODS), \
    $(call mod_detect_cycle_deps,$(DEPS-$(mod))) \
  )

# The real work is done here.
# This code is evaluated during symbol cache generation.
ifdef EMBUILD_DUMP_CREATE
SUBDIRS_LDFLAGS := $(__LDFLAGS)
PACKAGES := $(__PACKAGES)
$(__PACKAGES_PROCESS)
MODS_CORE := $(__MODS_CORE)
MODS := $(sort $(__MODS) $(MODS_CORE))# Essential mods are so essential...
LIBS := $(__LIBS)
$(__UNITS_PROCESS)
$(__MODS_PROCESS)
$(info Done.)
else
# If possible, get information from cache.
-include $(EMBUILD_DUMP_MK)
endif

# Perform units postprocessing: expand objects and define rules for them.

define define_unit_obj_rules
  OBJS-$(unit) := $(call SRC_TO_OBJ,$(SRCS-$(unit)))
  $(OBJS-$(unit)) : unit := $(unit)
  $(OBJS-$(unit)) : override CPPFLAGS += \
     $(CPPFLAGS-$(unit_package)) $(CPPFLAGS-$(unit)) $(CPPFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CFLAGS += \
           $(CFLAGS-$(unit_package)) $(CFLAGS-$(unit)) $(CFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CPPFLAGS := $(strip $(CPPFLAGS))
  $(OBJS-$(unit)) : override CFLAGS := $(strip $(CFLAGS))
endef
define define_lib_rules
  $(call LIB_FILE,$(unit)) : $(OBJS-$(unit))
	$(AR) $(ARFLAGS) $@ $(^:%= \$N	%)
endef
define define_mod_obj_rules
  $(OBJS-$(unit)) : override CPPFLAGS += -D__EMBUILD_MOD__='$(subst .,$$,$(unit))'
endef

$(foreach unit,$(LIBS), \
  $(eval $(value define_unit_obj_rules)) \
  $(eval $(value define_lib_rules)) \
)
$(foreach unit,$(MODS), \
  $(eval $(value define_unit_obj_rules)) \
  $(eval $(value define_mod_obj_rules)) \
)

# Now process mods that come from config files.
undefined_mods := $(filter-out $(MODS),$(MODS_ENABLE))
print_undefined_mods = \
  $(foreach mod,$(undefined_mods), \
    $(info $(call warning_str_file,$(AUTOCONF_DIR)/mods.mk) \
      Undefined mod $(mod)) \
  )

MODS_ENABLE := $(sort $(filter $(MODS),$(MODS_ENABLE)))

# Prepare the list of mods for the build.
MODS_BUILD := $(call MOD_DEPS_DAG,$(sort $(MODS_ENABLE) $(MODS_CORE)))

$(foreach mod,$(MODS_CORE), \
  $(eval RUNLEVEL-$(mod) := 0)\
)

# XXX just for now. -- Eldar
$(foreach mod,$(MODS_ENABLE),$(if $(RUNLEVEL-$(mod)),, \
  $(eval RUNLEVEL-$(mod) := 2)\
))

__print_units = \
  $(if $(foreach e,$2, \
    $(info ($1) $e $(if $3,	(defined in $(UNIT_DEFINED-$e))))x \
  ),,$(info (none)))

#print_units = \
  $(info All unints: ) \
  $(call __print_units,MOD,$(MODS),defined) \
  $(call __print_units,LIB,$(LIBS),defined)

# This is expanded in rule commands context
print_units += \
  $(info Mods enabled in config: ) \
  $(call __print_units,CONF,$(MODS_ENABLE)) \
  $(info Essential Mods: ) \
  $(call __print_units,CORE,$(filter-out $(MODS_ENABLE),$(MODS_CORE))) \
  $(info Mods included to satisfy dependencies: ) \
  $(call __print_units,DEPS,\
    $(filter-out $(MODS_ENABLE) $(MODS_CORE),$(MODS_BUILD)))

image_init:
	$(strip \
		$(print_undefined_mods) \
		$(do_not_print_units) \
	)

# Here goes dump generating stuff needed to speed-up a build.

EMBUILD_DUMP_PREREQUISITES += $(DIRS:%=%/Makefile)
EMBUILD_DUMP_PREREQUISITES += $(AUTOCONF_DIR)/build.mk

dump_var = $1 := $($1:%=\\\n  %)\n
dump_var_symbol = $(subst \\\n,\\\n  ,$(subst \n ,\n,$(strip \
  $(foreach var,$2,$(if $($1-$(var)),$(call dump_var,$1-$(var)))) \
)))

$(EMBUILD_DUMP_MK) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/embuild.mk
ifndef EMBUILD_DUMP_CREATE
	@$(RM) $@ && $(MAKE) EMBUILD_DUMP_CREATE=1 --no-print-directory $@
else
	@$(PRINTF) '# Auto-generated EMBuild symbols dump file. Do not edit.\n' > $@
	@$(PRINTF) '$(call dump_var,PACKAGES)' >> $@
	@$(PRINTF) '$(call dump_var,MODS_CORE)' >> $@
	@$(PRINTF) '$(call dump_var,MODS)' >> $@
	@$(PRINTF) '$(call dump_var,LIBS)' >> $@
	@$(PRINTF) '$(call dump_var_symbol,CPPFLAGS,$(PACKAGES))' >> $@
	@$(PRINTF) '$(call dump_var_symbol,CFLAGS,$(PACKAGES))' >> $@
	@$(PRINTF) '$(call dump_var_symbol,SRCS,$(MODS) $(LIBS))' >> $@
	@$(PRINTF) '$(call dump_var_symbol,CPPFLAGS,$(MODS) $(LIBS))' >> $@
	@$(PRINTF) '$(call dump_var_symbol,CFLAGS,$(MODS) $(LIBS))' >> $@
	@$(PRINTF) '$(call dump_var_symbol,DEPS,$(MODS))' >> $@
	@$(PRINTF) '$(call dump_var,SUBDIRS_LDFLAGS)' >> $@
	@$(PRINTF) '$(call dump_var_symbol,UNIT_DEFINED,$(MODS) $(LIBS))' >> $@
endif

endif
