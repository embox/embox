#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/traverse.mk
include $(MK_DIR)/util.mk

# Guard symbol. Use it only in contexts with $(dir) defined.
_ = EMBUILD/$(abspath $(dir))/

# Traverse always defines SELFDIR before entering sub-makefile.
dir = $(SELFDIR)

$(info Invoking traverse)
DIRS := $(call TRAVERSE,$(SRC_DIR)) \
  $(if $(PLATFORM),$(call TRAVERSE,$(PLATFORM_DIR)))

# Name check for packages and units.
invalid_symbols := ` ~ ! @ $$ % ^ & * ( ) { } [ ] < > ; : ' " \ | / ? + = № ..
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
$(info Processing package info)

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
PACKAGES := \
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

define define_package_symbols_per_directory
  CPPFLAGS-$(package) := $(CPPFLAGS-$(package)) \
     $(call package_symbol,CPPFLAGS)
  CFLAGS-$(package)   :=   $(CFLAGS-$(package)) \
       $(call package_symbol,CFLAGS)
endef

define define_package_symbols
  CPPFLAGS-$(package) := $(strip $(CPPFLAGS-$(package)) \
      $(call parent_package_symbol,CPPFLAGS))
  CFLAGS-$(package)   :=   $(strip $(CFLAGS-$(package)) \
        $(call parent_package_symbol,CFLAGS))
endef

$(foreach package,$(PACKAGES), \
  $(foreach dir,$(DIRS), \
    $(eval $(value define_package_symbols_per_directory)) \
  ) \
  $(eval $(value define_package_symbols)) \
)

# LDFLAGS are common for the entire image.
# Collect per-directory $_LDFLAGS definitions.
$(foreach dir,$(DIRS),$(eval override LDFLAGS := $(LDFLAGS) $($_LDFLAGS)))
override LDFLAGS := $(strip $(LDFLAGS))

$(info Listing mods and libs)

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
  $(foreach unit,$(call canonize_name,$1), \
    $(unit) $(eval $(unit)-DEFINED := $(dir)) \
  )

unit_symbol_collect = $(sort $(foreach dir,$(DIRS),$(call unit_def,$($_$1))))

# Mods that are always included to the resulting image
# (with their dependencies satisfied, of course).
MODS_ESSENTIAL := $(call unit_symbol_collect,MODS_ESSENTIAL)
# Regular mods.
MODS           := $(call unit_symbol_collect,MODS)
# Libraries.
LIBS           := $(call unit_symbol_collect,LIBS)

# Essential mods are so essential...
MODS := $(sort $(MODS) $(MODS_ESSENTIAL))

# Common units handling: source assignments and flags.
$(info Common unit processing)

wildcard_srcs = $(wildcard $(1:%=$(dir)/%))

unit_package = $(basename $(unit))
unit_name = $(patsubst .%,%,$(suffix $(unit)))

unit_symbol = $($_$1-$(unit)) \
  $(if $(filter $($_PACKAGE),$(unit_package)),$($_$1-$(unit_name)))

# Each source file should be assigned for a single unit. Prevent violation.
unit_srcs_check = \
  $(foreach src,$1, \
    $(if $(UNIT-$(abspath $(src))), \
      $(if $(filter $(UNIT-$(abspath $(src))),$(unit)), \
        $(info $(warning_str) \
          Repeated source assignment of $(src) to the same unit $(unit)) \
        $(info $(call warning_str_dir,$(UNIT-$(abspath $(src))-DEFINED))\
          first defined here), \
        $(info $(error_str) \
          Attempting to reassign source $(src) to unit $(unit) \
            (already assigned to $(UNIT-$(abspath $(src))))) \
        $(info $(call error_str_dir,$(UNIT-$(abspath $(src))-DEFINED))\
          first defined here) \
        $(error Multiple source assignment) \
      ), \
      $(eval UNIT-$(abspath $(src)) := $(unit)) \
      $(eval UNIT-$(abspath $(src))-DEFINED := $(dir)) \
      $(src) \
    ) \
  )

define define_unit_symbols_per_directory
  SRCS-$(unit) := $(SRCS-$(unit)) \
    $(call unit_srcs_check,$(call wildcard_srcs,$(call unit_symbol,SRCS)))
  CPPFLAGS-$(unit) := $(CPPFLAGS-$(unit)) $(call unit_symbol,CPPFLAGS)
  CFLAGS-$(unit)   :=   $(CFLAGS-$(unit)) $(call unit_symbol,CFLAGS)
endef

define define_unit_symbols
  SRCS-$(unit) := $(strip $(SRCS-$(unit)))
  OBJS-$(unit) := $(call SRC_TO_OBJ,$(SRCS-$(unit)))
  CPPFLAGS-$(unit) := $(strip $(CPPFLAGS-$(unit)) $(CPPFLAGS-$(unit_package)))
  CFLAGS-$(unit)   := $(strip   $(CFLAGS-$(unit))   $(CFLAGS-$(unit_package)))

  $(OBJS-$(unit)) : override CPPFLAGS := \
                        $(CPPFLAGS) $(CPPFLAGS-$(unit)) $(CPPFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CFLAGS := \
                        $(CFLAGS) $(CFLAGS-$(unit)) $(CFLAGS-$(abspath $@))
  -include $(OBJS-$(unit):.o=.d)

endef

$(foreach unit,$(MODS) $(LIBS), \
  $(foreach dir,$(DIRS), \
    $(eval $(value define_unit_symbols_per_directory)) \
  ) \
  $(eval $(value define_unit_symbols)) \
)

# Here goes libs handling stuff.
$(info Libs processing)
unit = $(lib)

lib_file = $(call LIB_FILE,$(lib))

define define_lib_rules

  $(lib_file) : OBJS := $(OBJS-$(lib))
  $(lib_file) : $(OBJS-$(lib))
	$(AR) $(ARFLAGS) $@ \
 $(OBJS:%=	% \$N)

endef

$(foreach lib,$(LIBS), \
  $(eval $(value define_lib_rules)) \
)

# Here goes mods specific functions (dependencies and so on).
$(info Mods processing)
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

define define_mod_symbols_per_directory
  DEPS-$(mod) := $(DEPS-$(mod)) \
    $(call mod_deps_filter,$(call canonize_name,$(call unit_symbol,DEPS)))
endef

# Define dependency info for each mod.
$(foreach mod,$(MODS), \
  $(foreach dir,$(DIRS), \
    $(eval $(value define_mod_symbols_per_directory)) \
  ) \
  $(eval DEPS-$(mod) := $(sort $(DEPS-$(mod)))) \
)

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

$(foreach mod,$(MODS),$(call mod_detect_cycle_deps,$(DEPS-$(mod))))

# Now process mods that come from config files.
$(info Checking configs)

MODS_ENABLE := $(sort $(MODS_ENABLE))
undefined_mods := $(filter-out $(MODS),$(MODS_ENABLE))
$(foreach mod,$(undefined_mods), \
  $(info $(call warning_str_file,$(AUTOCONF_DIR)/config.mk) \
    Undefined mod $(mod)) \
)
MODS_ENABLE := $(filter $(MODS),$(MODS_ENABLE))

# Prepare the list of mods for the build.
MODS_BUILD := $(call MOD_DEPS_DAG,$(sort $(MODS_ENABLE) $(MODS_ESSENTIAL)))

#$(foreach mod,$(MODS), \
  $(info (GLOBAL) $(mod) defined in $($(mod)-DEFINED)) \
)
$(info Mods enabled in config: )
$(foreach mod,$(MODS_ENABLE), \
  $(info (CONFIG) $(mod)) \
)
$(info Essential Mods: )
$(foreach mod,$(filter-out $(MODS_ENABLE),$(MODS_ESSENTIAL)), \
  $(info (ESSENT) $(mod)) \
)
$(info Mods included to satisfy dependencies: )
$(foreach mod,$(filter-out $(MODS_ENABLE) $(MODS_ESSENTIAL),$(MODS_BUILD)), \
  $(info (DEPEND) $(mod)) \
)

