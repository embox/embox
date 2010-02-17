#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/traverse.mk
include $(MK_DIR)/util.mk

# Guard symbol. Use it only in contexts with $(dir) defined.
_ = EMBUILD/$(abspath $(dir))/

# Traverse always defines SELFDIR before entering sub-makefile.
dir = $(SELFDIR)
DIRS := $(call TRAVERSE,$(SRC_DIR)) \
  $(if $(PLATFORM_DIR),$(call TRAVERSE,$(PLATFORM_DIR)))

dir_package_lookup = \
  $(if $(filter $(abspath $(ROOT_DIR))%,$1),$(foreach dir,$1, \
    $(eval $_PACKAGE ?= $(strip $(call $0,$(dir $(1:%/=%))))) \
    $(eval $_PACKAGE := $($_PACKAGE)) \
    $(eval $_PACKAGE := $(if $($_PACKAGE),$($_PACKAGE),generic)) \
    $($_PACKAGE) \
  ))

# Get package list.
# This also provokes every $_PACKAGE variable to be explicitly defined.
PACKAGES := \
  $(sort $(foreach dir,$(DIRS),$(call dir_package_lookup,$(abspath $(dir)))))

# Canonical unit name is one with package prefix.
canonize_name = \
  $(foreach name,$1, \
    $(if $(findstring .,$(name)), \
      $(if $(or $(filter .%,$(name)),$(filter %.,$(name))),,$(name)), \
      $($_PACKAGE).$(name) \
    ) \
  )

# Get list of all modules and libraries.
MODS := $(sort $(foreach dir,$(DIRS),$(call canonize_name,$($_MODS))))
LIBS := $(sort $(foreach dir,$(DIRS),$(call canonize_name,$($_LIBS))))

# Mods that are always included to the resulting image
# (with their dependencies satisfied, of course).
MODS_ESSENTIAL := \
  $(sort $(foreach dir,$(DIRS),$(call canonize_name,$($_MODS_ESSENTIAL))))

# Essential mods are so essential...
MODS := $(sort $(MODS) $(MODS_ESSENTIAL))

# Common units handling: source assignments and flags.

error_string_remote = $1/Makefile:0: EMBuild error::
warning_string_remote = $1/Makefile:0: EMBuild warning::

error_string = $(call error_string_remote,$(dir))
warning_string = $(call warning_string_remote,$(dir))

wildcard_srcs = $(wildcard $(1:%=$(dir)/%))

unit_package = $(basename $(unit))
unit_name = $(patsubst .%,%,$(suffix $(unit)))

unit_symbol = $($_$1-$(unit)) \
  $(if $(filter $($_PACKAGE),$(unit_package)),$($_$1-$(unit_name)))

package_symbol = $(if $(filter $($_PACKAGE),$(unit_package)),$($_$1))

# Each source file should be assigned for a single unit. Prevent violation.
unit_srcs_check = \
  $(foreach src,$1, \
    $(if $(UNIT-$(abspath $(src))), \
      $(if $(filter $(UNIT-$(abspath $(src))),$(unit)), \
        $(info $(warning_string) \
          Repeated source assignment of $(src) to the same unit $(unit)) \
        $(info $(call warning_string_remote,$(UNIT-$(abspath $(src))-DEFINED))\
          first defined here), \
        $(info $(error_string) \
          Attempting to reassign source $(src) to unit $(unit) \
            (already assigned to $(UNIT-$(abspath $(src))))) \
        $(info $(call error_string_remote,$(UNIT-$(abspath $(src))-DEFINED))\
          first defined here) \
        $(error Multiple source assignment) \
      ), \
      $(eval UNIT-$(abspath $(src)) := $(unit)) \
      $(eval UNIT-$(abspath $(src))-DEFINED := $(dir)) \
      $(src) \
    ) \
  )

define define_common_unit_symbols_per_directory
  SRCS-$(unit) := $(SRCS-$(unit)) \
    $(call unit_srcs_check,$(call wildcard_srcs,$(call unit_symbol,SRCS)))
  CPPFLAGS-$(unit) := $(CPPFLAGS-$(unit)) \
     $(call unit_symbol,CPPFLAGS) $(call package_symbol,CPPFLAGS)
  CFLAGS-$(unit)   :=   $(CFLAGS-$(unit)) \
       $(call unit_symbol,CFLAGS) $(call package_symbol,CFLAGS)
#  LDFLAGS-$(unit)  :=  $(LDFLAGS-$(unit)) \
      $(call unit_symbol,LDFLAGS) $(call package_symbol,LDFLAGS)
endef

define define_common_unit_symbols
  SRCS-$(unit) := $(strip $(SRCS-$(unit)))
  OBJS-$(unit) := $(call SRC_TO_OBJ,$(SRCS-$(unit)))
  CPPFLAGS-$(unit) := $(strip $(CPPFLAGS-$(unit)))
  CFLAGS-$(unit)   :=   $(strip $(CFLAGS-$(unit)))
#  LDFLAGS-$(unit)  :=  $(strip $(LDFLAGS-$(unit)))

  $(OBJS-$(unit)) : override CPPFLAGS := \
                        $(CPPFLAGS) $(CPPFLAGS-$(unit)) $(CPPFLAGS-$(abspath $@))
  $(OBJS-$(unit)) : override CFLAGS := \
                        $(CFLAGS) $(CFLAGS-$(unit)) $(CFLAGS-$(abspath $@))
  -include $(OBJS-$(unit):.o=.d)

endef

$(foreach unit,$(MODS) $(LIBS), \
  $(foreach dir,$(DIRS), \
    $(eval $(value define_common_unit_symbols_per_directory)) \
  ) \
  $(eval $(value define_common_unit_symbols)) \
)

# Here goes mods specific functions (dependencies and so on).
unit = $(mod)

# The sub-graph of all module dependencies (either direct or indirect).
# TODO Add cyclic deps check
MOD_DEPS_DAG = $(sort $(call mod_deps_dag_walk,$1))
mod_deps_dag_walk = $(foreach mod,$1,$(call $0,$(DEPS-$(mod))) $(mod))

# User should list only existing mods in dependency list. Check it is true.
mod_deps_filter = \
  $(foreach dep, \
    $(if $(filter-out $(MODS),$1), \
      $(info $(warning_string) Undefined dependencies for mod $(mod):: \
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
      $(info $(call error_string_remote,$(MOD-$(subst /,-DEP-,$(pair))-DEFINED)) \
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

#mod_check_inheritance = \
  $(call mod_detect_multiple_inheritance,$1) \
  $(call mod_detect_inheritance_loop,$(mod))

# Param: mod children
#mod_detect_multiple_inheritance = \
  $(foreach child_mod,$1, \
    $(if $(filter-out $(mod),$(PARENT-$(child_mod))), \
      $(error EMBuild mods error $N \
        Multiple inheritance detected for mod $(child_mod): $N \
        old parent: $(PARENT-$(child_mod)), \
          (defined in $(PARENT_DEFINED-$(child_mod))) $N \
        new parent: $(mod), \
          (defined in $(dir))), \
      $(eval PARENT-$(child_mod) := $(mod)) \
      $(eval PARENT_DEFINED-$(child_mod) := $(dir)/Makefile) \
    ) \
  )

# Param: mod parent
#mod_detect_inheritance_loop = \
  $(if $2,,$(call $0,$1,$N ^- <root>)) \
  $(if $(filter $(mod),$(PARENT-$1)), \
    $(error EMBuild mods error $N \
      Inheritance loop detected for mod $(mod): \
      $N- $1 $2 (defined in $(PARENT_DEFINED-$1))), \
    $(if $1, $(call $0,$(PARENT-$1), \
      $N ^- $1 (defined in $(PARENT_DEFINED-$1)) $(2:$N=$N .))) \
  )

#mod_tree_walk_process = \
  $(foreach mod,$1, \
    $(foreach dir,$(DIRS), \
      $(eval $(value mod_symbols_per_directory)) \
      $(call mod_check_inheritance,$($_MODS-$(mod))) \
    ) \
    $(eval $(value mod_symbols)) \
    $(call $0,$(MODS-$(mod))) \
  )

# Here goes libs handling stuff.
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

