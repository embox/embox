
.PHONY : all include_install include_path_install

all: include_install include_path_install

include mk/flags.mk
include mk/image_lib.mk

INCLUDE_INSTALL := include_install

src_file =
include_install_files =
include_install_prerequisites = $(include_install_files) $(src_file) $(common_prereqs)

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(MKGEN_DIR)/include_install.rule.mk

include $(__include_incinst)

cp_T_if_supported := $(shell $(CP) --version 2>&1 | grep -l GNU >/dev/null && echo -T)

############################
$(INCLUDE_INSTALL_DIR)/%/. : | $(INCLUDE_INSTALL_DIR)/.
	@$(MKDIR) $(@D)
	@touch $@

$(INCLUDE_INSTALL_DIR)/% : | $(INCLUDE_INSTALL_DIR)/.
	$(CP) -r $(cp_T_if_supported) $(src_file) $@

$(INCLUDE_INSTALL_DIR)/. :
	@$(MKDIR) $(@D)
#################################

############################
srcgen_include_path := $(addprefix $(SRCGEN_DIR)/src/,$(SRC_INCLUDE_PATH))
include_path_install : $(srcgen_include_path)

$(srcgen_include_path) :
	@$(MKDIR) $@

ifdef GEN_DIST
include_path_install :
	cd $(SRC_DIR) && $(CP) -r --parents $(SRC_INCLUDE_PATH) $(SRCGEN_DIR)/src
endif # GEN_DIST
############################
