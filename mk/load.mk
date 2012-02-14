#
#   Date: Jan 25, 2012
# Author: Eldar Abusalimov
#

MK_CACHE_DIR := mk/.cache/mk

# Default values which we are about to override with target-specific ones.
export CACHE_INCLUDES :=
export CACHE_REQUIRES :=

# Because inclusions of cached scripts may occur in a non-linear order
# we need to specify a unique allocation scope to prevent possible collisions.
# (Generally speaking the order of caching is defined by the dependence DAG of
# the cache. Thus if script A requires B and C, which are cached
# independently and allocate the same resource, then a collision would happen
# after including both of them at the same time.)
export ALLOC_SCOPE

# Core scripts: def & obj.
export mk_core_def := $(MK_CACHE_DIR)/mk_core_def.mk
$(mk_core_def) : CACHE_INCLUDES := \
	mk/core/define.mk
$(mk_core_def) : ALLOC_SCOPE := a

export mk_core_obj := $(MK_CACHE_DIR)/mk_core_obj.mk
$(mk_core_obj) : CACHE_INCLUDES := \
	mk/core/object.mk
$(mk_core_obj) : CACHE_REQUIRES := \
	$(mk_core_def)
$(mk_core_obj) : ALLOC_SCOPE := b

# GOLD parser engine.
export mk_gold_engine := $(MK_CACHE_DIR)/mk_gold_engine.mk
$(mk_gold_engine) : CACHE_INCLUDES := \
	mk/gold/engine.mk
$(mk_gold_engine) : CACHE_REQUIRES := \
	$(mk_core_def)
$(mk_gold_engine) : ALLOC_SCOPE := c

ifeq (1,0) ###
# Tiny version of EMF Ecore.
export mk_model := $(MK_CACHE_DIR)/mk_model.mk
$(mk_model) : CACHE_INCLUDES := \
	mk/model/model.mk     \
	mk/model/factory.mk   \
	mk/model/metamodel.mk \
	mk/model/resource.mk  \
	mk/model/linkage.mk
$(mk_model) : CACHE_REQUIRES := \
	$(mk_core_obj)
$(mk_model) : ALLOC_SCOPE := d

# Mybuild itself.
export mk_mybuild := $(MK_CACHE_DIR)/mk_mybuild.mk
$(mk_mybuild) : CACHE_INCLUDES := \
	mk/mybuild/myfile-model.mk     \
	mk/mybuild/myfile-factory.mk   \
	mk/mybuild/myfile-metamodel.mk \
	mk/mybuild/myfile-resource.mk  \
	mk/mybuild/myfile-parser.mk
$(mk_mybuild) : CACHE_REQUIRES := \
	$(mk_core_def) \
	$(mk_gold_engine) \
	$(mk_model)
$(mk_mybuild) : ALLOC_SCOPE := e

all_mk_scripts := \
	$(mk_core_def) \
	$(mk_core_obj) \
	$(mk_gold_engine) \
	$(mk_model) \
	$(mk_mybuild)

else ###

export mk_mybuild := $(MK_CACHE_DIR)/mk_mybuild.mk
$(mk_mybuild) : CACHE_INCLUDES := \
	mk/mybuild/model.mk     \
	mk/mybuild/myfile.mk    \
	mk/mybuild/resource.mk  \
	mk/mybuild/mybuild.mk
$(mk_mybuild) : CACHE_REQUIRES := \
	$(mk_core_obj) \
	$(mk_gold_engine)
$(mk_mybuild) : ALLOC_SCOPE := e

all_mk_scripts := \
	$(mk_core_def) \
	$(mk_core_obj) \
	$(mk_gold_engine) \
	$(mk_mybuild)

endif ###

export all_mk_scripts

$(MAKECMDGOALS) : $(all_mk_scripts)
	@$(MAKE) -f mk/load2.mk $@

-include $(all_mk_scripts:%=%.d)

.DELETE_ON_ERROR:
.SECONDEXPANSION:
$(all_mk_scripts) : $$(CACHE_INCLUDES)
$(all_mk_scripts) : $$(CACHE_REQUIRES)
$(all_mk_scripts) : mk/load.mk
$(all_mk_scripts) : mk/script/mk-cache.mk
$(all_mk_scripts) :
	@echo ' MKGEN $(@F)'
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-cache.mk CACHE_DEP_TARGET='$@' > $@
