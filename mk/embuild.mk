#
# Author: Eldar Abusalimov
#

ifndef _embuild_mk_
_embuild_mk_ := 1

#EMBUILD_DEBUG = 1

ifdef EMBUILD_DEBUG
$(info Running EMBuild [$(MAKELEVEL)])
endif

include $(MK_DIR)/traverse.mk
include $(MK_DIR)/util.mk

EMBUILD_DUMP_MK := $(CODEGEN_DIR)/embuild_cache.mk

# Guard symbol. Use it only in contexts with $(dir) defined.
ifndef EMBUILD_DEBUG
_ = EMBUILD/$(abspath $(dir))/
else
_ = $(if $(dir),EMBUILD/$(abspath $(dir))/, \
      $(error using guard symbol outside dir scope, dir value is $(value dir)))
endif

# Traverse always defines SELFDIR before entering sub-makefile.
dir = $(SELFDIR)

DIRS := $(call traverse,$(SRC_DIR),Makefile.em) \
  $(if $(PLATFORM),$(call TRAVERSE,$(PLATFORM_DIR),Makefile.em))

ifdef EMBUILD_DEBUG
embuild_vars = $(filter EMBUILD/%,$(.VARIABLES))
$(foreach v,$(embuild_vars), \
  $(info $v = $($v)) \
)
endif

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

# Get list of all modules and libraries.
unit_def = \
  $(foreach unit,$1, \
    $(unit) $(eval UNIT_DEFINED-$(unit) := $(dir)) \
  )

# Mods that are always included to the resulting image
# (with their dependencies satisfied, of course).
__MODS_CORE = $(info Listing core mods) $(call mod_collect,MODS_CORE)
# Regular mods.
__MODS      = $(info Listing mods) $(call mod_collect,MODS)

mod_collect = $(sort \
  $(foreach dir,$(DIRS),$(call unit_def,$(call canonize_mod_name,$($_$1)))) \
)

# Canonical mod name is one with package prefix.
canonize_mod_name = \
  $(foreach name,$1, \
    $(call check_name,$(name)) \
    $(if $(findstring .,$(name)), \
      $(name), \
      $($_PACKAGE).$(name) \
    ) \
  )

# Libraries.
__LIBS = $(info Listing libs) $(call lib_collect,LIBS)

lib_collect = $(sort $(foreach dir,$(DIRS),$(call unit_def,$($_$1))))

# Common units handling: source assignments and flags.

wildcard_srcs = $(wildcard $(1:%=$(dir)/%))

mod_package = $(basename $(unit))
mod_name = $(patsubst .%,%,$(suffix $(unit)))

unit_symbol = $(foreach symbol,$1, \
  $($_$(symbol)-$(unit)) \
  $(if $(filter $($_PACKAGE),$(mod_package)),$($_$(symbol)-$(mod_name))) \
)

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

__UNITS_PROCESS = $(info Processing units) \
  $(foreach unit,$(MODS) $(LIBS), \
    $(eval $(value define_unit_symbols_prepare)) \
    $(foreach dir,$(DIRS), \
      $(eval $(value define_unit_symbols_per_directory)) \
    ) \
    $(eval $(value define_unit_symbols)) \
  )

# Deal with mods and libs dependencies
symbol_dag_walk = $(foreach u,$1,$u $(call $0,$($2-$u),$2))

# User should list only existing units in dependency list. Check it is true.
# Params:
#  1. Unit dependencies
#  2. Set of allowed dependency values
unit_deps_filter = \
  $(foreach dep, \
    $(or $(and $(filter-out $2,$1), \
               $(info $(warning_str) Undefined dependencies for unit $(unit):: \
                 $(filter-out $2,$1)) $(filter $2,$1) \
         ),$1), \
    $(eval UNIT-$(unit)-DEP-$(dep)-DEFINED := $(dir)) \
    $(dep) \
  )

# Mod deps are curiously enough simple.

mod_deps_per_directory = $(call unit_deps_filter, \
  $(call canonize_mod_name,$(call unit_symbol,DEPS)),$(MODS) \
)
define define_mod_deps_per_directory
  DEPS-$(unit) := $(DEPS-$(unit)) $(mod_deps_per_directory)
endef
define define_mod_deps
  DEPS-$(unit) := $(sort $(DEPS-$(unit)))
endef

invoke_define_mod_deps = \
  $(foreach unit,$(MODS), \
    $(foreach dir,$(DIRS),$(eval $(value define_mod_deps_per_directory))) \
    $(eval $(value define_mod_deps)) \
  )

# Libs are more complicated because of the need to deal with USES/SUBS stuff.

lib_uses_per_directory = \
  $(call unit_deps_filter,$(call unit_symbol,USES),$(LIBS))
lib_subs_per_directory = \
  $(call unit_deps_filter,$(call unit_symbol,SUBS),$(LIBS))

define define_lib_deps_per_directory
  USES-$(unit) := $(USES-$(unit)) $(lib_uses_per_directory)
  SUBS-$(unit) := $(SUBS-$(unit)) $(lib_subs_per_directory)
endef
define define_lib_deps
  USES-$(unit) := $(sort $(USES-$(unit)))
  SUBS-$(unit) := $(sort $(SUBS-$(unit)))
  DEPS-$(unit) := $(USES-$(unit)) $(SUBS-$(unit))
endef

invoke_define_lib_deps = \
  $(foreach unit,$(LIBS), \
    $(foreach dir,$(DIRS),$(eval $(value define_lib_deps_per_directory))) \
    $(eval $(value define_lib_deps)) \
  )

lib_subs_super_graph = \
  $(foreach l,$(LIBS),$(if $(filter $1,$(call symbol_dag_walk,$l,SUBS)),$l))

define define_lib_deps_postprocess
  DEPS-$(unit) := $(sort $(DEPS-$(unit)) \
    $(call lib_subs_super_graph,$(USES-$(unit))))
endef

invoke_define_lib_deps_postprocess = \
  $(foreach unit,$(LIBS), \
    $(eval $(value define_lib_deps_postprocess)) \
  )

# After dependency info has been collected for all units we should check that
# the dependency graphs are true DAGs.
deps_detect_cycles = \
  $(if $(filter $(unit),$1), \
    $(foreach pair, \
        $(call unit_dep_pairs,$2 $(lastword $3),$3 $(firstword $2)), \
      $(info $(call error_str_dir,$(UNIT-$(subst /,-DEP-,$(pair))-DEFINED)) \
        Cyclic dependency definition here:: $(subst /, -> ,$(pair))) \
    ) \
    $(error Dependency cycle:: $(unit) $(strip $(3:%= -> %))), \
    $(foreach parent,$(if $(u),$(u),$(unit)),$(foreach u,$1,\
      $(call $0,$(DEPS-$(u)), \
        $2 $(parent),$3 $(u)) \
    )) \
  ) # I don't understand anymore how does this shit work but it does, trust me.
unit_dep_pairs = $(join $(1:%=%/),$(2))

# Define dependency info for each unit and then perform graph check.
__DEPS_PROCESS = $(info Processing dependencies) \
  $(invoke_define_mod_deps) \
  $(invoke_define_lib_deps) \
  $(foreach unit,$(MODS) $(LIBS),$(call deps_detect_cycles,$(DEPS-$(unit)))) \
  $(invoke_define_lib_deps_postprocess) \
  $(foreach unit,$(LIBS),$(call deps_detect_cycles,$(DEPS-$(unit))))

# The sub-graph of all module dependencies (either direct or indirect).
MOD_DEPS_DAG = $(sort $(call symbol_dag_walk,$1,DEPS))

# Params:
#  1. List of units remaining unsorted
#  2. List of already sorted units
t-sort = \
  $(if $(strip $1), \
    $(call $0, \
      $(filter-out $2,$1), \
      $2 $(filter-out $2,$(foreach n,$1,$(if $(filter-out $2,$(DEPS-$n)),,$n))) \
    ), \
    $(strip $2) \
  )

reverse = \
  $(strip $(if $1,$(call $0,$(wordlist 2,$(words $1),$1)) $(firstword $1)))

define mod_deps_resort
  DEPS-$(mod) := $(foreach m,$(MODS),$(if $(filter $m,$(DEPS-$(mod))),$m))
endef

__MOD_DEPS_RESORT = $(foreach mod,$(MODS),$(eval $(value mod_deps_resort)))

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
$(__DEPS_PROCESS)
MODS := $(call t-sort,$(MODS))
LIBS := $(call reverse,$(call t-sort,$(LIBS)))
$(__MOD_DEPS_RESORT)
$(info Done.)
else
# If possible, get information from cache.
ifneq ($(wildcard $(EMBUILD_DUMP_MK)),)
include $(EMBUILD_DUMP_MK)
else
-include $(EMBUILD_DUMP_MK)
endif
endif

# Perform units postprocessing: expand objects and define rules for them.

define define_unit_obj_rules
  OBJS-$(unit) := $(call SRC_TO_OBJ,$(SRCS-$(unit)))
  $(OBJS-$(unit)) : unit := $(unit)
  $(OBJS-$(unit)) : override CPPFLAGS += \
                                $(CPPFLAGS-$(unit)) $(CPPFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CFLAGS += \
                                    $(CFLAGS-$(unit)) $(CFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CPPFLAGS := $(strip $(CPPFLAGS))
  $(OBJS-$(unit)) : override CFLAGS := $(strip $(CFLAGS))
endef
define define_lib_rules
  $(call LIB_FILE,$(unit)) : $(OBJS-$(unit))
	$(AR) $(ARFLAGS) $@ $(^:%= \$N	%)
endef
define define_mod_obj_rules
  $(OBJS-$(unit)) : override CPPFLAGS += $(CPPFLAGS-$(mod_package))
  $(OBJS-$(unit)) : override CFLAGS   += $(CFLAGS-$(mod_package))
  $(OBJS-$(unit)) : override CPPFLAGS += -D__EMBUILD_MOD__='$(subst .,__,$(unit))'
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

# XXX just for now, agrh... -- Eldar
$(foreach mod,$(MODS_ENABLE),$(if $(RUNLEVEL-$(mod)),, \
  $(eval RUNLEVEL-$(mod) := 2)\
))

ifdef EMBUILD_DEBUG
__print_units = \
  $(if $(foreach e,$2, \
    $(info ($1) $e $(if $3,	(defined in $(UNIT_DEFINED-$e))))x \
  ),,$(info (none)))

print_units = \
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
endif

image_init:
	$(strip \
		$(print_undefined_mods) \
		$(print_units) \
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
	@$(PRINTF) '$(call dump_var_symbol,DEPS,$(MODS) $(LIBS))' >> $@
	@$(PRINTF) '$(call dump_var,SUBDIRS_LDFLAGS)' >> $@
	@$(PRINTF) '$(call dump_var_symbol,UNIT_DEFINED,$(MODS) $(LIBS))' >> $@
endif

endif
