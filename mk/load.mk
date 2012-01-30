#
#   Date: Jan 25, 2012
# Author: Eldar Abusalimov
#

CACHE_DIR := mk/cache

# Core scripts: def & obj.
mk_core := $(CACHE_DIR)/mk_core.mk
$(mk_core)/includes := \
	mk/core/string.mk \
	mk/core/define.mk \
	mk/core/object.mk
$(mk_core)/uses :=

# GOLD parser engine.
mk_gold := $(CACHE_DIR)/mk_gold.mk
$(mk_gold)/includes := \
	mk/gold/engine.mk
$(mk_gold)/uses := \
	$(mk_core)

# Tiny version of EMF Ecore.
mk_model := $(CACHE_DIR)/mk_model.mk
$(mk_model)/includes := \
	mk/model/model.mk     \
	mk/model/factory.mk   \
	mk/model/metamodel.mk
$(mk_model)/uses := \
	$(mk_core)

# Mybuild itself.
mk_mybuild := $(CACHE_DIR)/mk_mybuild.mk
$(mk_mybuild)/includes := \
	mk/mybuild/myfile-model.mk     \
	mk/mybuild/myfile-factory.mk   \
	mk/mybuild/myfile-metamodel.mk \
	mk/mybuild/myfile-resource.mk  \
	mk/mybuild/myfile-parser.mk
$(mk_mybuild)/uses := \
	$(mk_core) \
	$(mk_gold) \
	$(mk_model)

all_mk_targets := \
	$(mk_core) \
	$(mk_gold) \
	$(mk_model) \
	$(mk_mybuild)

all : $(all_mk_targets)
	$(MAKE) -f mk/core/common.mk MAKEFILES=$(mk_mybuild)

.SECONDEXPANSION:
$(all_mk_targets) : $$($$@/includes)
$(all_mk_targets) : $$($$@/uses)
$(all_mk_targets) : mk/load.mk
$(all_mk_targets) : mk/cache.mk
	@mkdir -p $(@D)
	$(MAKE) -f mk/cache.mk \
		CACHE_INCLUDES='$(value $@/includes)' \
		CACHE_USES='$(value $@/uses)' > $@
