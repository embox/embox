#
# Prepares necessary Make scripts.
#
#   Date: Jan 25, 2012
# Author: Eldar Abusalimov
#

ifndef CACHE_DIR
$(error CACHE_DIR is not defined, \
	do not include this script directly, use mk/load.mk instead)
endif

MK_CACHE_DIR := $(CACHE_DIR)/mk

# Core scripts: def & obj.
mk_core_def := $(MK_CACHE_DIR)/mk_core_def.mk
$(mk_core_def) : CACHE_INCLUDES := \
	mk/core/define.mk
$(mk_core_def) : ALLOC_SCOPE := a
load_mk_files += $(mk_core_def)

mk_core_obj := $(MK_CACHE_DIR)/mk_core_obj.mk
$(mk_core_obj) : CACHE_INCLUDES := \
	mk/core/object.mk
$(mk_core_obj) : CACHE_REQUIRES := \
	$(mk_core_def)
$(mk_core_obj) : ALLOC_SCOPE := b
load_mk_files += $(mk_core_obj)

# Utils.
mk_util := $(MK_CACHE_DIR)/mk_util.mk
$(mk_util) : CACHE_INCLUDES := \
	mk/util/wildcard.mk \
	mk/util/graph.mk
$(mk_util) : CACHE_REQUIRES := \
	$(mk_core_def)
$(mk_util) : ALLOC_SCOPE := c
load_mk_files += $(mk_util)

# GOLD parser engine.
mk_gold_engine := $(MK_CACHE_DIR)/mk_gold_engine.mk
$(mk_gold_engine) : CACHE_INCLUDES := \
	mk/gold/engine.mk
$(mk_gold_engine) : CACHE_REQUIRES := \
	$(mk_core_def)
$(mk_gold_engine) : ALLOC_SCOPE := d
load_mk_files += $(mk_gold_engine)

# Tiny version of EMF Ecore.
mk_model := $(MK_CACHE_DIR)/mk_model.mk
$(mk_model) : CACHE_INCLUDES := \
	mk/model/model.mk     \
	mk/model/metamodel.mk \
	mk/model/resource.mk  \
	mk/model/linkage.mk
$(mk_model) : CACHE_REQUIRES := \
	$(mk_core_obj)
$(mk_model) : ALLOC_SCOPE := e
load_mk_files += $(mk_model)

mk_annotations_core := $(MK_CACHE_DIR)/mk_annotations_core.mk
$(mk_annotations_core) : CACHE_INCLUDES := \
	mk/mybuild/annotations_core.mk
$(mk_annotations_core) : CACHE_REQUIRES := \
	$(mk_model)
$(mk_annotations_core) : ALLOC_SCOPE := ac
load_mk_files += $(mk_annotations_core)


mk_annotations_handlers := $(wildcard $(ANNOTATION_HANDLERS)/*.mk)
mk_annotations_handlers_mk := $(MK_CACHE_DIR)/mk_annotations_handlers.mk
$(mk_annotations_handlers_mk) : CACHE_INCLUDES := \
	$(mk_annotations_handlers)
$(mk_annotations_handlers_mk) : CACHE_REQUIRES := \
	$(mk_annotations_core)
$(mk_annotations_handlers_mk) : ALLOC_SCOPE := ah
load_mk_files += $(mk_annotations_handlers_mk)

# Myfiles parser & model.
mk_mybuild_myfile := $(MK_CACHE_DIR)/mk_mybuild_myfile.mk
$(mk_mybuild_myfile) : CACHE_INCLUDES := \
	mk/mybuild/myfile-model.mk     \
	mk/mybuild/myfile-metamodel.mk \
	mk/mybuild/myfile-resource.mk  \
	mk/mybuild/myfile-linker.mk  \
	mk/mybuild/myfile-parser.mk
$(mk_mybuild_myfile) : CACHE_REQUIRES := \
	$(mk_gold_engine) \
	$(mk_annotations_core) \
	$(mk_model)
$(mk_mybuild_myfile) : ALLOC_SCOPE := f
load_mk_files += $(mk_mybuild_myfile)

# Configuration files model.
mk_mybuild_configfile := $(MK_CACHE_DIR)/mk_mybuild_configfile.mk
$(mk_mybuild_configfile) : CACHE_INCLUDES := \
	mk/mybuild/configfile-model.mk     \
	mk/mybuild/configfile-metamodel.mk \
	mk/mybuild/configfile-resource.mk  \
	mk/mybuild/configfile-linker.mk    \
	mk/mybuild/configfile-parser.mk
$(mk_mybuild_configfile) : CACHE_REQUIRES := \
	$(mk_mybuild_myfile) \
	$(mk_model)
$(mk_mybuild_configfile) : ALLOC_SCOPE := g
load_mk_files += $(mk_mybuild_configfile)

# Build model files.
mk_mybuild_build := $(MK_CACHE_DIR)/mk_mybuild_build.mk
$(mk_mybuild_build) : CACHE_INCLUDES := \
	mk/mybuild/build-model.mk     \
	mk/mybuild/build-metamodel.mk
$(mk_mybuild_build) : CACHE_REQUIRES := \
	$(mk_mybuild_myfile) \
	$(mk_mybuild_configfile) \
	$(mk_model)
$(mk_mybuild_build) : ALLOC_SCOPE := h
load_mk_files += $(mk_mybuild_build)

# Mybuild itself.
mk_mybuild := $(MK_CACHE_DIR)/mk_mybuild.mk
$(mk_mybuild) : CACHE_INCLUDES := \
	mk/mybuild/mybuild.mk
$(mk_mybuild) : CACHE_REQUIRES := \
	$(mk_mybuild_myfile)     \
	$(mk_mybuild_configfile) \
	$(mk_mybuild_build)
$(mk_mybuild) : ALLOC_SCOPE := i
load_mk_files += $(mk_mybuild)

# Ugly scripts.
mk_ugly := $(MK_CACHE_DIR)/mk_ugly.mk
$(mk_ugly) : CACHE_INCLUDES := \
	mk/ugly.mk
$(mk_ugly) : CACHE_REQUIRES := \
	$(mk_mybuild)# Agrrr...
$(mk_ugly) : ALLOC_SCOPE := u
load_mk_files += $(mk_ugly)

# Flatten and export.
export load_mk_files := $(load_mk_files)

#
# To get our scripts work properly an 'ALLOC_SCOPE' variable should be defined.
#
# Different scopes are necessary to prevent possible collisions, because
# inclusions of cached scripts may occur in a non-linear order. Generally
# speaking, the order of caching is defined by the dependence DAG of the cache.
# Thus if script A requires B and C, which are cached independently and
# allocate the same resource, then a collision would happen after including
# both of them at the same time.
$(load_mk_files) : export ALLOC_SCOPE ?=

# Default values which are overridden with target-specific ones.
$(load_mk_files) : export CACHE_INCLUDES ?=
$(load_mk_files) : export CACHE_REQUIRES ?=

$(load_mk_files) : $$(CACHE_INCLUDES)
$(load_mk_files) : $$(CACHE_REQUIRES)
$(load_mk_files) : mk/load-mk.inc.mk
$(load_mk_files) : mk/script/mk-cache.mk
$(load_mk_files) : | $$(@D)/.
$(load_mk_files) :
	@echo ' MKGEN $(@F)'
	@$(MAKE) -f mk/script/mk-cache.mk CACHE_DEP_TARGET='$@' > $@

-include $(load_mk_files:%=%.d)
