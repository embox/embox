#
# Author: Eldar Abusalimov
#

CC      =$(CC_PACKET)-gcc
OD_TOOL =$(CC_PACKET)-objdump
OC_TOOL =$(CC_PACKET)-objcopy

CCINCLUDES =-I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include -nostdinc

LDSCRIPT =$(SRC_DIR)/arch/$(ARCH)/embox.lds

TARGET_DIS  = $(TARGET).dis
TARGET_SREC = prom.srec

.PHONY: image checksum

IMAGE_TARGET = $(BUILD_DIR)/$(TARGET)
IMAGE_SREC = $(BUILD_DIR)/$(TARGET_SREC)
ifeq ($(DISASSEMBLE),y)
IMAGE_DISASSEMBLE = $(BUILD_DIR)/$(TARGET_DIS)
endif
ifeq ($(CHECKSUM),y)
IMAGE_CHECKSUM = checksum
endif

image: $(IMAGE_TARGET) $(IMAGE_SREC) $(IMAGE_DISASSEMBLE) $(IMAGE_CHECKSUM)
	@echo 'Build complete'

# TODO actually not all objects depend on config.h -- Eldar
#$(OBJS_ALL): $(BUILD_DIR)/config.h

# TODO ... but $(TARGET) does not depend at config.h at all -- Eldar
$(BUILD_DIR)/$(TARGET): $(BUILD_DIR)/config.h $(OBJS_ALL) $(LDSCRIPT)
	@echo '$(CC) $(LDFLAGS) -T $(LDSCRIPT) -o $@ \ '
	@echo '	<<a lot of object files>>'
	@$(CC) $(LDFLAGS) -T $(LDSCRIPT) -o $@ $(OBJS_ALL)
#	@echo '$(OBJS_ALL)' | sed 's/ /\n/g'

$(BUILD_DIR)/$(TARGET_DIS): $(BUILD_DIR)/$(TARGET)
	$(OD_TOOL) -S $< > $@

$(BUILD_DIR)/$(TARGET_SREC): $(BUILD_DIR)/$(TARGET)
	$(OC_TOOL) -O srec $< $@

checksum:
#	@$(MAKE) --directory=scripts/md5_checksummer
#	@if [ $(CHECKSUM) == y ]; \
#	then \
#	    $(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#			-o $(OC_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD); \
#	    declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; \
#			$(MAKE) --directory=src all; \
#	else \
#	    $(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#			-o $(OC_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean;\
#	fi;
