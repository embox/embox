#
# Author: Eldar Abusalimov
#

IMAGE      = $(BIN_DIR)/$(TARGET)
IMAGE_DIS  = $(IMAGE).dis
IMAGE_BIN  = $(IMAGE).bin
IMAGE_SREC = $(IMAGE).srec
IMAGE_SIZE = $(IMAGE).size
IMAGE_PIGGY= $(IMAGE).piggy

.PHONY: image image_init image_fini
image: image_init
image: $(IMAGE)
image: $(IMAGE_DIS)
image: $(IMAGE_BIN)
image: $(IMAGE_SREC)
image: $(IMAGE_SIZE)
image: $(IMAGE_PIGGY)
image: image_fini

image_init image_fini:

.PHONY: image_prepare
prepare: image_prepare

image_prepare:
	@mkdir -p $(OBJ_SUBDIRS) 2>/dev/null || echo No objs are built

CC      = $(CROSS_COMPILE)gcc
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = $(CROSS_COMPILE)size


#check the gcc version
CC_VERSION = $(shell $(CC) -v 2>&1 | grep -e "^gcc" | cut -d' ' -f3 )

ifeq ($(strip $(CC_VERSION)),)
$(error Unable to get GCC version: $(shell $(CC) -v 2>&1 | cat))
endif
CC_VERSION_MAJOR := $(shell echo $(CC_VERSION) | cut -d'.' -f 1)
CC_VERSION_MINOR := $(shell echo $(CC_VERSION) | cut -d'.' -f 2)

ifneq ($(or $(filter-out 0 1 2 3 4,$(CC_VERSION_MAJOR)), \
       $(and $(filter 4,$(CC_VERSION_MAJOR)), \
             $(filter-out 0 1,$(CC_VERSION_MINOR)))),)
CC_SUPPORTS_@file := 1
endif

#ifeq ($(C),1)
#CHECK   = sparse
#REAL_CC = $(CC)
#CC     := cgcc
#endif

include mk/flags.mk

# It's time to scan subdirs and prepare mods info.
# ...and to build dependency injection model
include mk/codegen-di.mk

APIS_BUILD := $(call filter_abstract_modules,$(MODS_ENABLE_OBJ))
LIBS_BUILD := $(call filter_static_modules,$(MODS_ENABLE_OBJ))
MODS_BUILD := $(filter-out $(LIBS_BUILD),$(MODS_ENABLE_OBJ))

SRCS_BUILD := \
	$(foreach m,$(MODS_ENABLE_OBJ),$(call module_get_sources,$m))
OBJS_BUILD := $(call SRC_TO_OBJ,$(SRCS_BUILD))
SRCS_BUILD_NONLIB := \
	$(foreach m,$(MODS_BUILD),$(call module_get_sources,$m))
OBJS_BUILD_NONLIB := $(call SRC_TO_OBJ,$(SRCS_BUILD_NONLIB))
LDSS_BUILD := \
	$(call filter-patsubst,\
		$(abspath $(ROOT_DIR))/%.lds.S,\
		$(OBJ_DIR)/%.lds,\
		$(abspath $(SRCS_BUILD)))

ROOTFS_SRCS_BUILD := \
	$(foreach m,$(MODS_ENABLE_OBJ),$(call module_get_rootfs,$m))

rootfs_src_to_obj = $(addprefix $(ROOTFS_DIR)/,$(notdir $1))

$(foreach src,$(ROOTFS_SRCS_BUILD),\
	$(eval \
		$(call rootfs_src_to_obj,$(src)) : $(src) \
		$(\n)$(\t)@$(MKDIR) $$(@D) && cp -T $$< $$@))
#XXX
$(OBJ_DIR)/src/fs/ramfs/ramfs_cpio.o : $(ROOTFS_IMAGE)

ROOTFS_OBJS_BUILD := $(call rootfs_src_to_obj,$(ROOTFS_SRCS_BUILD))

$(ROOTFS_DIR) :
	@mkdir $@

$(ROOTFS_IMAGE): $(ROOTFS_DIR) $(ROOTFS_OBJS_BUILD)
	@pushd $< && \
		find . -depth -print | \
	       	cpio --quiet -H newc -o > ../$(notdir $@); \
		popd

ifdef LDSS_BUILD
LD_SINGLE_T_OPTION := \
	$(if $(value LD_SINGLE_T_OPTION),$(filter-out n,$(LD_SINGLE_T_OPTION)))
ifndef LD_SINGLE_T_OPTION
override LDFLAGS += $(LDSS_BUILD:%=-T %)
else
override LDFLAGS += -T $(LDSS_BUILD)
endif
endif

$(foreach m,$(MODS_ENABLE_OBJ),$(call define_mod_obj_rules,$m))
$(foreach l,$(LIBS_BUILD),$(call define_lib_rules,$l))

-include $(patsubst %.lds,%.lds.d,$(OBJS_BUILD:.o=.d))

#OBJS_BUILD := $(foreach mod,$(MODS_BUILD),$(OBJS-$(mod)))
OBJ_SUBDIRS := \
	$(sort $(dir $(OBJS_BUILD) $(LDSS_BUILD)))

include mk/headers.mk

$(OBJS_BUILD) $(LDSS_BUILD): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk

__CMDS = \
  $(patsubst $(abspath $(ROOT_DIR))%.$1,$(OBJ_DIR)%.cmd,$(filter %.$1,$(abspath $(SRCS_BUILD))))

CMDS_C := $(call __CMDS,c)
CMDS_S := $(call __CMDS,S)

CMDS := $(CMDS_C) $(CMDS_S)

$(CMDS_C) : __FLAGS = $(CFLAGS) $(CPPFLAGS) $(CCFLAGS)
$(CMDS_S) : __FLAGS = $(ASFLAGS) $(CPPFLAGS) $(CCFLAGS)

$(CMDS) : FLAGS = $(subst ",,$(__FLAGS))
$(CMDS) :
	@echo '$(FLAGS) -o $(@:%.cmd=%.o) -c' > $@

$(CMDS): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk mk/image.mk

ifndef VERBOSE
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

$(OBJ_DIR)/%.lds :: $(ROOT_DIR)/%.lds.S $(config_lds_h)
	mkdir -p $(@D)
	$(CPP) -P -undef $(CPPFLAGS) -imacros $(AUTOCONF_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

ifndef PARTIAL_LINKING

$(IMAGE): $(HEADERS_BUILD) $(DEPSINJECT_OBJ) $(OBJS_BUILD) $(LDSS_BUILD) $(call LIB_FILE,$(LIBS_BUILD))
	$(LD) $(LDFLAGS) $(OBJS_BUILD_NONLIB:%=\$(\n)		%) \
		$(DEPSINJECT_OBJ) \
		$(patsubst %,\$(\n)		%,$(call LIB_FILE,$(LIBS_BUILD))) \
	-Map $@.map \
	-o   $@

else

IMAGE_O    = $(IMAGE).o
$(IMAGE_O): $(DEPSINJECT_OBJ) $(OBJS_BUILD) $(LDSS_BUILD) $(call LIB_FILE,$(LIBS_BUILD))
	$(LD) -r -o  $@ $(OBJS_BUILD_NONLIB:%=\$(\n)	%) \
	$(DEPSINJECT_OBJ)
$(IMAGE): $(IMAGE_O)
	$(LD) $(LDFLAGS) $< \
	$(patsubst %,\$(\n)		%,$(call LIB_FILE,$(LIBS_BUILD))) \
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

