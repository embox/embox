#
#   Date: Nov 15, 2025
# Author: Aleksey Zhmulin
#

embox_o   := $(OBJ_DIR)/embox-2.o
embox_a   := $(OBJ_DIR)/embox-2.a
image_lds := $(OBJ_DIR)/image.lds

include mk/flags.mk

dist_gen_timestamp := $(BUILD_BASE_DIR)/dist-gen.timestamp

.PHONY : all
all: $(dist_gen_timestamp)

$(dist_gen_timestamp) : $(embox_o) $(embox_a)
	$(CP) -r $(INCLUDE_INSTALL_DIR) $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/include $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/arch/$(ARCH)/include $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/compat/linux/include $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/compat/posix/include $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/compat/bsd/include $(DIST_DIR)
	$(CP) -r $(SRC_DIR)/compat/libc/include $(DIST_DIR)
	$(CP) $(embox_o) $(DIST_LIB_DIR)/embox.o
	$(CP) $(embox_a) $(DIST_LIB_DIR)/embox.a
	$(CP) $(image_lds) $(DIST_LIB_DIR)/image.lds
	touch $@
