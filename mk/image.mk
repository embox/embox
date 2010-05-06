#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/util.mk

IMAGE      = $(BIN_DIR)/$(TARGET)
IMAGE_DIS  = $(BIN_DIR)/$(TARGET).dis
IMAGE_SREC = $(BIN_DIR)/$(TARGET).srec
IMAGE_SIZE = $(BIN_DIR)/$(TARGET).size

.PHONY: image
image: image_init
image: $(IMAGE) $(IMAGE_SREC) $(IMAGE_SIZE)
ifeq ($(DISASSEMBLE),y)
image: $(IMAGE_DIS)
endif
ifeq ($(CHECKSUM),y)
image: checksum
endif
image: image_fini

image_init image_fini:

.PHONY: image_prepare
prepare: image_prepare
image_prepare:
	@mkdir -p $(OBJ_SUBDIRS)

.PHONY: checksum
checksum:
# Not yet implemented

CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = size

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags:=$(CPPFLAGS)
override CPPFLAGS  = -D__EMBOX__
override CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.h
override CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include
override CPPFLAGS += -nostdinc
override CPPFLAGS += -MMD -MP# -MT $@ -MF $(@:.o=.d)
override CPPFLAGS += $(cppflags)

# Compiler flags
cflags:=$(CFLAGS)
override CFLAGS  = -pedantic
override CFLAGS += -fno-strict-aliasing -fno-common
override CFLAGS += -Wall
override CFLAGS += -Wstrict-prototypes -Wdeclaration-after-statement -Winline
override CFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CFLAGS += -pipe
override CFLAGS += $(cflags)

# Linker flags
ldflags:=$(LDFLAGS)
override LDFLAGS  = -static
override LDFLAGS += -nostdlib
override LDFLAGS += -T $(LDSCRIPT)
override LDFLAGS += $(SUBDIRS_LDFLAGS)
override LDFLAGS += $(ldflags)

override ARFLAGS = rcs

LDSCRIPT = $(OBJ_DIR)/$(TARGET).lds

SRC_TO_OBJ = $(patsubst $(ROOT_DIR)%,$(OBJ_DIR)%.o,$(basename $1))
LIB_FILE   = $(1:%=$(LIB_DIR)/%)

# It's time to scan subdirs and prepare mods info.
include $(MK_DIR)/embuild.mk
# ...and to build dependency injection model
include $(MK_DIR)/codegen-di.mk

OBJS_ALL := $(foreach unit,$(MODS) $(LIBS),$(OBJS-$(unit)))
-include $(OBJS_ALL:.o=.d)

OBJS_BUILD := $(foreach mod,$(MODS_BUILD),$(OBJS-$(mod)))
OBJ_SUBDIRS := \
  $(sort $(dir $(OBJS_BUILD) $(foreach lib,$(LIBS),$(OBJS-$(lib)))))

$(OBJS_ALL): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk

$(OBJ_DIR)/%.o::$(ROOT_DIR)/%.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) -std=gnu99 -c $<

$(OBJ_DIR)/%.o::$(ROOT_DIR)/%.S
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) -c $<

$(IMAGE): $(DEPSINJECT_OBJ) $(OBJS_BUILD) $(call LIB_FILE,$(LIBS))
	$(LD) $(LDFLAGS) $(OBJS_BUILD:%=\$N		%) \
		$(DEPSINJECT_OBJ) \
	-L$(LIB_DIR) $(LIBS:lib%.a=\$N		-l%) \
	-o $@ -M > $@.map

$(IMAGE_DIS): $(IMAGE)
	@$(OBJDUMP) -S $< > $@

$(IMAGE_SREC): $(IMAGE)
	@$(OBJCOPY) -O srec $< $@

define image_size_sort
	@echo "" >> $@
	@echo "sort by text $2" >> $@
	@cat $@.tmp | sort -g -k $1 >> $@
endef

$(IMAGE_SIZE): $(IMAGE) $(OBJS_BUILD) $(DEPSINJECT_OBJ)
	@$(SIZE) $^ > $@.tmp
	@echo "size util generated output for $(TARGET)" > $@
	$(call image_size_sort,1,text)
	$(call image_size_sort,2,data)
	$(call image_size_sort,3,bss)
	$(call image_size_sort,4,total)
	@$(RM) $@.tmp

