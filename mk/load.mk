#
#   Date: Jan 25, 2012
# Author: Eldar Abusalimov
#

CACHE_DIR := mk/.cache

# Core scripts: def & obj.
export mk_core_def := $(CACHE_DIR)/mk_core_def.mk
$(mk_core_def) : CACHE_INCLUDES := \
	mk/core/define.mk

export mk_core_obj := $(CACHE_DIR)/mk_core_obj.mk
$(mk_core_obj) : CACHE_INCLUDES := \
	mk/core/object.mk
$(mk_core_obj) : CACHE_REQUIRES := \
	$(mk_core_def)

# GOLD parser engine.
export mk_gold_engine := $(CACHE_DIR)/mk_gold_engine.mk
$(mk_gold_engine) : CACHE_INCLUDES := \
	mk/gold/engine.mk
$(mk_gold_engine) : CACHE_REQUIRES := \
	$(mk_core_def)

ifeq (0,1) ###
# Tiny version of EMF Ecore.
export mk_model := $(CACHE_DIR)/mk_model.mk
$(mk_model) : CACHE_INCLUDES := \
	mk/model/model.mk     \
	mk/model/factory.mk   \
	mk/model/metamodel.mk
$(mk_model) : CACHE_REQUIRES := \
	$(mk_core_def)

# Mybuild itself.
export mk_mybuild := $(CACHE_DIR)/mk_mybuild.mk
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

all_mk_scripts := \
	$(mk_core_def) \
	$(mk_core_obj) \
	$(mk_gold_engine) \
	$(mk_model) \
	$(mk_mybuild)

else ###

export mk_mybuild := $(CACHE_DIR)/mk_mybuild.mk
$(mk_mybuild) : CACHE_INCLUDES := \
	mk/mybuild/model.mk     \
	mk/mybuild/myfile.mk     \
	mk/mybuild/resource.mk
$(mk_mybuild) : CACHE_REQUIRES := \
	$(mk_core_obj) \
	$(mk_gold_engine)

all_mk_scripts := \
	$(mk_core_def) \
	$(mk_core_obj) \
	$(mk_gold_engine) \
	$(mk_mybuild)

endif ###

# Defaults.
export CACHE_INCLUDES :=
export CACHE_REQUIRES :=

$(MAKECMDGOALS) : $(all_mk_scripts)
	@$(MAKE) -f mk/main.mk MAKEFILES=$(mk_mybuild) $@

-include $(all_mk_scripts:%=%.d)

.SECONDEXPANSION:
$(all_mk_scripts) : $$(CACHE_INCLUDES)
$(all_mk_scripts) : $$(CACHE_REQUIRES)
$(all_mk_scripts) : mk/load.mk
$(all_mk_scripts) : mk/cache.mk
$(all_mk_scripts) :
	@echo Preparing $(@F)...
	@mkdir -p $(@D) && $(MAKE) -f mk/cache.mk CACHE_DEP_TARGET='$@' > $@
