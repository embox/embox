#
#   Date: Jun 22, 2012
# Author: Eldar Abusalimov
#

.PHONY : all image
all : image

include mk/core/common.mk

include $(MKGEN_DIR)/build.mk
include mk/flags.mk # It must be included after a user-defined config.

.SECONDEXPANSION:
include $(MKGEN_DIR)/include.mk

.SECONDARY:
.DELETE_ON_ERROR:

TARGET ?= embox$(if $(value PLATFORM),-$(PLATFORM))
TARGET := $(TARGET)$(if $(value LOCALVERSION),-$(LOCALVERSION))

IMAGE       = $(BIN_DIR)/image
IMAGE_DIS   = $(IMAGE).dis
IMAGE_BIN   = $(IMAGE).bin
IMAGE_SREC  = $(IMAGE).srec
IMAGE_SIZE  = $(IMAGE).size
IMAGE_PIGGY = $(IMAGE).piggy

image: $(IMAGE)
image: $(IMAGE_DIS) $(IMAGE_BIN) $(IMAGE_SREC) $(IMAGE_SIZE) $(IMAGE_PIGGY)

CROSS_COMPILE ?=

CC      := $(CROSS_COMPILE)gcc
CPP     := $(CC) -E
CXX     := $(CROSS_COMPILE)g++
AR      := $(CROSS_COMPILE)ar
AS      := $(CROSS_COMPILE)as
LD      := $(CROSS_COMPILE)ld
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

# This must be expanded in a secondary expansion context.
common_prereqs = mk/image2.mk mk/flags.mk $(MKGEN_DIR)/build.mk $(mk_file)

VPATH = $(SRCGEN_DIR)

%/. :
	@$(MKDIR) $*

cc_prerequisites    = $(common_prereqs)

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.c | $$(@D)/.
	$(CC) $(CFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.S | $$(@D)/.
	$(CC) $(ASFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cpp | $$(@D)/.
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<
$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cxx | $$(@D)/.
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

cpp_prerequisites   = $(common_prereqs)
$(OBJ_DIR)/%.lds : $(ROOT_DIR)/%.lds.S | $$(@D)/.
	$(CPP) -P -undef $(CPPFLAGS) -imacros $(SRCGEN_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

initfs_cp_prerequisites = $(common_prereqs) $(src_file)
$(ROOTFS_DIR)/% : | $(ROOTFS_DIR)/.
	@$(CP) -T $(src_file) $@
$(ROOTFS_DIR)/. :
	@$(MKDIR) $(@D)

fmt_line = $(addprefix \$(\n)$(\t)$(\t),$1)

initfs_prerequisites = $(common_prereqs) $(cpio_files)
$(ROOTFS_IMAGE) : rel_cpio_files = \
		$(patsubst $(abspath $(ROOTFS_DIR))/%,%,$(abspath $(cpio_files)))
$(ROOTFS_IMAGE) : | $$(@D)/.
	cd $(ROOTFS_DIR) \
		&& ls -1R $(rel_cpio_files) | cpio --quiet -H newc -o -O $(abspath $@)
	cd $(USER_ROOTFS_DIR) \
		&& ls -1R * || exit 0 | cpio --quiet -H newc -o --append -O $(abspath $@)
#XXX
$(OBJ_DIR)/src/fs/ramfs/ramfs_cpio.o : $(ROOTFS_IMAGE)

ar_prerequisites    = $(common_prereqs) $(ar_objs)
$(OBJ_DIR)/%.a : | $$(@D)/.
	$(AR) $(ARFLAGS) $@ $(call fmt_line,$(ar_objs))

image_prerequisites = $(mk_file) \
	$(ld_scripts) $(ld_objs) $(ld_libs)
$(IMAGE): | $$(@D)/.
	$(LD) $(LDFLAGS) $(call fmt_line,$(ld_scripts:%=-T%)) \
	$(call fmt_line,$(ld_objs)) \
	$(call fmt_line,$(ld_libs)) \
	-Map $@.map \
	-o   $@

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

$(IMAGE_SIZE): $(IMAGE)
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

