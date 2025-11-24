#
#   Date: Nov 15, 2025
# Author: Aleksey Zhmulin
#

include mk/flags.mk

ifdef PLATFORM
dist_name := embox-$(ARCH)-$(PLATFORM)
else
dist_name := embox-$(ARCH)-unknown
endif

dist_dir     := $(DIST_INSTALL_DIR)/$(dist_name)
dist_archive := $(DIST_INSTALL_DIR)/$(dist_name).tar.gz

.PHONY : all
all: $(dist_archive)

$(dist_dir) : $(DIST_INC_DIR) $(DIST_LIB_DIR) $(DIST_BIN_DIR)
	$(RM) -r $(dist_dir)
	$(MKDIR) $(dist_dir)
	$(CP) -r $(DIST_INC_DIR) $(dist_dir)
	$(CP) -r $(DIST_LIB_DIR) $(dist_dir)
	$(CP) -r $(DIST_BIN_DIR) $(dist_dir)

$(dist_archive) : $(dist_dir)
	tar -czf $@ -C $(DIST_INSTALL_DIR) $(dist_name)
