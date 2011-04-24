#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/util.mk

IMAGE      = $(BIN_DIR)/$(TARGET)
IMAGE_DIS  = $(IMAGE).dis
IMAGE_BIN  = $(IMAGE).bin
IMAGE_SREC = $(IMAGE).srec
IMAGE_SIZE = $(IMAGE).size
IMAGE_PIGGY= $(IMAGE).piggy

.PHONY: image image_init image_fini
image: image_init
image: $(IMAGE) $(IMAGE_SREC) $(IMAGE_BIN) $(IMAGE_SIZE) $(IMAGE_PIGGY)
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
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = $(CROSS_COMPILE)size

CC_VERSION := \
  $(shell $(CC) -v 2>&1 | grep "gcc version" | cut -d' ' -f3)
ifeq ($(strip $(CC_VERSION)),)
$(error Unable to get GCC version: $(shell $(CC) -v 2>&1 | cat))
endif
CC_VERSION_MAJOR := $(word 1,$(subst ., ,$(CC_VERSION)))
CC_VERSION_MINOR := $(word 2,$(subst ., ,$(CC_VERSION)))
CC_VERSION_PATCH := $(word 3,$(subst ., ,$(CC_VERSION)))

ifneq ($(or $(call gt,$(CC_VERSION_MAJOR),4), \
       $(and $(call  eq,$(CC_VERSION_MAJOR),4), \
             $(call gte,$(CC_VERSION_MINOR),2))),)
CC_SUPPORTS_@file := 1
endif

ifeq ($(C),1)
CHECK   = sparse
REAL_CC = $(CC)
CC     := cgcc
endif

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags := $(CPPFLAGS)
override CPPFLAGS  = -D__EMBOX__
override CPPFLAGS += -D"__impl(path)=<impl/path>"
override CPPFLAGS += -D"__impl_x(path)=<../path>"
override CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.h
override CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include
override CPPFLAGS += -nostdinc
override CPPFLAGS += -MMD -MP# -MT $@ -MF $(@:.o=.d)
override CPPFLAGS += $(cppflags)

# Assembler flags
asflags := $(CFLAGS)
override ASFLAGS  = -pipe
override ASFLAGS += $(asflags)

# Compiler flags
cflags := $(CFLAGS)
override CFLAGS  = -pedantic -std=gnu99
override CFLAGS += -fno-strict-aliasing -fno-common
override CFLAGS += -Wall -Werror
override CFLAGS += -Wstrict-prototypes -Wdeclaration-after-statement -Winline
override CFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CFLAGS += -pipe
override CFLAGS += $(cflags)

# Linker flags
ldflags := $(LDFLAGS)
override LDFLAGS  = -static
override LDFLAGS += -nostdlib
override LDFLAGS += --cref --relax
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
SRCS_ALL := $(foreach unit,$(MODS) $(LIBS),$(SRCS-$(unit)))
-include $(OBJS_ALL:.o=.d)

OBJS_BUILD := $(foreach mod,$(MODS_BUILD),$(OBJS-$(mod)))
OBJ_SUBDIRS := \
  $(sort $(dir $(OBJS_BUILD) $(foreach lib,$(LIBS),$(OBJS-$(lib)))))

$(OBJS_ALL): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk

__CMDS = \
  $(patsubst $(ROOT_DIR)%.$1,$(OBJ_DIR)%.cmd,$(filter %.$1,$(SRCS_ALL)))

CMDS_C := $(call __CMDS,c)
CMDS_S := $(call __CMDS,S)

CMDS := $(CMDS_C) $(CMDS_S)

$(CMDS_C) : __FLAGS = $(CFLAGS) $(CPPFLAGS)
$(CMDS_S) : __FLAGS = $(ASFLAGS) $(CPPFLAGS)
$(CMDS) : FLAGS = $(subst ",,$(__FLAGS))
$(CMDS) :
	@echo '$(FLAGS) -o $(@:%.cmd=%.o) -c' > $@

ifndef __CDT__
ifdef CC_SUPPORTS_@file
CC_RULES = $(CC) @$< $(word 2,$^)
else
CC_RULES = $(CC) `cat $<` $(word 2,$^)
endif
else
CC_RULES = $(CC) $(patsubst -D%,-D"%",$(shell cat $<)) $(word 2,$^)
endif

$(OBJ_DIR)/%.o :: $(OBJ_DIR)/%.cmd $(ROOT_DIR)/%.c
	$(CC_RULES)

$(OBJ_DIR)/%.o :: $(OBJ_DIR)/%.cmd $(ROOT_DIR)/%.S
	$(CC_RULES)

ifndef PARTIAL_LINKING

$(IMAGE): $(DEPSINJECT_OBJ) $(OBJS_BUILD) $(call LIB_FILE,$(LIBS))
	$(LD) $(LDFLAGS) $(OBJS_BUILD:%=\$(\n)		%) \
		$(DEPSINJECT_OBJ) \
	-L$(LIB_DIR) $(LIBS:lib%.a=\$(\n)		-l%) \
	-Map $@.map \
	-o   $@

else

IMAGE_O    = $(IMAGE).o
$(IMAGE_O): $(DEPSINJECT_OBJ) $(OBJS_BUILD) $(call LIB_FILE,$(LIBS))
	$(LD) -r -o  $@ $(OBJS_BUILD:%=\$(\n)	%) \
	$(DEPSINJECT_OBJ)
$(IMAGE): $(IMAGE_O)
	$(LD) $(LDFLAGS) $< \
	-L$(LIB_DIR) $(LIBS:lib%.a=\$(\n)		-l%) \
	-Map $@.map \
	-o   $@

endif

$(IMAGE_DIS): $(IMAGE)
	@$(OBJDUMP) -S $< > $@

$(IMAGE_SREC): $(IMAGE)
	@$(OBJCOPY) -O srec $< $@

$(IMAGE_BIN): $(IMAGE)
	@$(OBJCOPY) -O binary $< $@

$(IMAGE_PIGGY): $(IMAGE)
	@$(OBJCOPY) -O binary -R .note -R .comment -S $< $@.tmp
	@$(LD) -r -b binary $@.tmp -o $@
	@$(RM) $@.tmp

image_size_sort = \
	echo "" >> $@;                    \
	echo "sort by $2 size" >> $@;     \
	cat $@.tmp | sort -g -k $1 >> $@;

$(IMAGE_SIZE): $(IMAGE) $(OBJS_BUILD) $(DEPSINJECT_OBJ)
	@if [ `which $(SIZE) 2> /dev/null` ];  \
	then                                   \
	    $(SIZE) $^ > $@.tmp;               \
		echo "size util generated output for $(TARGET)" > $@; \
		$(call image_size_sort,1,text)     \
		$(call image_size_sort,2,data)     \
		$(call image_size_sort,3,bss)      \
		$(call image_size_sort,4,total)    \
		$(RM) $@.tmp;                      \
	else                                   \
		echo "$(SIZE) util not found" > $@;   \
	fi;

