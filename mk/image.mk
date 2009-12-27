#
# Author: Eldar Abusalimov
#

include $(MK_DIR)/traverse.mk

CC     =$(CROSS_COMPILE)gcc
AR     =$(CROSS_COMPILE)ar
AS     =$(CROSS_COMPILE)as
OBJDUMP=$(CROSS_COMPILE)objdump
OBJCOPY=$(CROSS_COMPILE)objcopy

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags:=$(CPPFLAGS)
CPPFLAGS =
CPPFLAGS+=-I$(SRC_DIR)/include
CPPFLAGS+=-I$(SRC_DIR)/arch/$(ARCH)/include
CPPFLAGS+=-nostdinc
CPPFLAGS+=-MMD# -MT $@ -MF $(@:.o=.d)
CPPFLAGS+=$(cppflags)

# Compiler flags
cflags:=$(CFLAGS)
CFLAGS =
CFLAGS+=-Werror
CFLAGS+=-pipe
CFLAGS+=$(cflags)

# Linker flags
ldflags:=$(LDFLAGS)
LDFLAGS =
LDFLAGS+=-static
LDFLAGS+=-nostdlib
LDFLAGS+=-T $(LDSCRIPT)
LDFLAGS+=$(ldflags)

LDLIBS=-L$(LIB_DIR) -l$(LIB_NAME)

ARFLAGS=rcs

LIB_NAME:=c
LIBC=$(LIB_DIR)/lib$(LIB_NAME).a

LDSCRIPT =$(OBJ_DIR)/arch/$(ARCH)/embox.lds

IMAGE=$(BIN_DIR)/$(TARGET)
IMAGE_DIS =$(BIN_DIR)/$(TARGET).dis
IMAGE_SREC=$(BIN_DIR)/prom.srec

# Just for better output readability.
define NEW_LINE


endef

# Clear some variables.
# Switch them to immediate expansion mode to be able to use += operator later.
OBJS_ALL:=
LIBS_ALL:=
DIRS_ALL:=
TRGS_ALL:=

# This code is executed each time when per-directory makefile is processed.
define TRAVERSE_CALLBACK
  obj_node_dir:=$(NODE_DIR:$(SRC_DIR)%=$(OBJ_DIR)%)
  DIRS_ALL+=$$(obj_node_dir)
  OBJS_ALL+=$$(addprefix $$(obj_node_dir)/,$(NODE_OBJS))
  LIBS_ALL+=$$(addprefix $$(obj_node_dir)/,$(NODE_LIBS))
  TRGS_ALL+=$(NODE_TARGETS)
endef

# Walk the directory tree starting at $(SRC_DIR)
# and searching for Makefile in each sub-directory.
$(eval $(call TRAVERSE,$(SRC_DIR),Makefile,TRAVERSE_CALLBACK))

# Expand these variables stripping out unnecessary whitespaces.
DIRS_ALL:=$(strip $(DIRS_ALL))
OBJS_ALL:=$(strip $(OBJS_ALL))
LIBS_ALL:=$(strip $(LIBS_ALL))
TRGS_ALL:=$(strip $(TRGS_ALL))

# Process dependency files.
-include $(OBJS_ALL:.o=.d)

# TODO actually not all objects depend on config.h -- Eldar
#$(OBJS_ALL): $(BUILDCONF_DIR)/config.h
# TODO ... but $(TARGET) does not depend at config.h at all -- Eldar
TRGS_ALL+=$(BUILDCONF_DIR)/config.h

$(OBJ_DIR)/%.o::$(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	-c -o $@ $<

$(OBJ_DIR)/%.o::$(SRC_DIR)/%.S
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	-c -o $@ $<

$(IMAGE): $(TRGS_ALL) $(OBJS_ALL) $(LIBC)
	$(CC) $(LDFLAGS) \
 $(patsubst %,	% \$(NEW_LINE),$(OBJS_ALL)) $(LDLIBS) -o $@

$(LIBC): $(LIBS_ALL)
	$(AR) $(ARFLAGS) $@ \
 $(patsubst %,	% \$(NEW_LINE),$^)

$(IMAGE_DIS): $(IMAGE)
	$(OBJDUMP) -S $< > $@

$(IMAGE_SREC): $(IMAGE)
	$(OBJCOPY) -O srec $< $@

.PHONY: image
image: $(IMAGE) $(IMAGE_SREC)
ifeq ($(DISASSEMBLE),y)
image: $(IMAGE_DIS)
endif
ifeq ($(CHECKSUM),y)
image: checksum
endif
image:
	@echo 'Build complete'

.PHONY: checksum
checksum:
#	@$(MAKE) --directory=scripts/md5_checksummer
#	@if [ $(CHECKSUM) == y ]; \
#	then \
#	    $(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#			-o $(OBJCOPY) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD); \
#	    declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; \
#			$(MAKE) --directory=src all; \
#	else \
#	    $(SCRIPTS_DIR)/ConfigBuilder/Misc/checksum.py \
#			-o $(OBJCOPY) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean;\
#	fi;
