#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/util.mk
include $(MK_DIR)/rules.mk

IMAGE      = $(BIN_DIR)/$(TARGET)
IMAGE_DIS  = $(BIN_DIR)/$(TARGET).dis
IMAGE_SREC = $(BIN_DIR)/$(TARGET).srec

.PHONY: image
image: $(IMAGE) $(IMAGE_SREC)
ifeq ($(DISASSEMBLE),y)
image: $(IMAGE_DIS)
endif
ifeq ($(CHECKSUM),y)
image: checksum
endif

.PHONY: image_prepare
prepare: image_prepare
image_prepare:
	@mkdir -p $(sort $(dir $(OBJS_ALL)))

.PHONY: checksum
checksum:
# Not yet implemented

CC      = $(CROSS_COMPILE)gcc
AR      = $(CROSS_COMPILE)ar
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags:=$(CPPFLAGS)
override CPPFLAGS =
# XXX
override CPPFLAGS += -I$(THIRDPARTY_DIR)/linux-compat/include
override CPPFLAGS += -D__EMBOX__
override CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.h
override CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include
override CPPFLAGS += -nostdinc
override CPPFLAGS += -MMD# -MT $@ -MF $(@:.o=.d)
override CPPFLAGS += $(cppflags)

# Compiler flags
cflags:=$(CFLAGS)
override CFLAGS =
override CFLAGS += -pedantic
override CFLAGS += -Wall
override CFLAGS += -Wstrict-prototypes -Wdeclaration-after-statement -Winline
override CFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CFLAGS += -pipe
override CFLAGS += $(cflags)

# Linker flags
ldflags:=$(LDFLAGS)
override LDFLAGS =
override LDFLAGS += -static
override LDFLAGS += -nostdlib
override LDFLAGS += -T $(LDSCRIPT)
override LDFLAGS += $(ldflags)

override ARFLAGS = rcs

LDLIBS = -L$(LIB_DIR) $(LIBS:%=-l.%)

LDSCRIPT = $(BIN_DIR)/$(TARGET).lds

SRC_TO_OBJ = $(patsubst $(ROOT_DIR)%,$(OBJ_DIR)%.o,$(basename $1))
MOD_FILE   = $(1:%=$(MOD_DIR)/%.o)
LIB_FILE   = $(1:%=$(LIB_DIR)/lib.%.a)

# It's time to build dependency model!
include $(MK_DIR)/embuild.mk

OBJS_ALL := $(foreach unit,$(MODS) $(LIBS),$(OBJS-$(unit)))
MODS_ENABLE := $(sort $(MODS_ENABLE) $(MODS_ESSENTIAL))

$(OBJS_ALL): $(AUTOCONF_DIR)/config.h

$(OBJ_DIR)/%.o::$(ROOT_DIR)/%.c
	$(CC) -o $@ \
	$(CPPFLAGS) $(CFLAGS) -std=gnu99 -c $<

$(OBJ_DIR)/%.o::$(ROOT_DIR)/%.S
	$(CC) -o $@ \
	$(CPPFLAGS) $(CFLAGS) -c $<

#$(info Enabled mods DAG: $(patsubst %,$N%,$(call MOD_DEPS_DAG,$(MODS_ENABLE))))
MOD_FILES = $(call MOD_FILE,$(call MOD_DEPS_DAG,$(MODS_ENABLE)))

$(IMAGE): $(call MOD_FILE,$(MODS_ENABLE)) $(call LIB_FILE,$(LIBS))
	$(CC) $(LDFLAGS) -o $@ $(MOD_FILES:%= \$N	%) \
	$(LDLIBS)

$(IMAGE_DIS): $(IMAGE)
	$(OBJDUMP) -S $< > $@

$(IMAGE_SREC): $(IMAGE)
	$(OBJCOPY) -O srec $< $@

