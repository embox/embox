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
$(warning Unable to get GCC version: $(shell $(CC) -v 2>&1 | cat)$(\n)\
You may ignore this message if CROSS_COMPILE was changed)
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

#build_model := $(mybuild_get_active_build)

MODS_ENABLE_OBJ := \
	$(if $(call invoke,$(call get,$(build_model),issueReceiver),getIssues),\
		$(call printIssues,$(build_model)),\
		$(call listInstances,$(build_model)))

#$(warning $(call printInstances,$(build_model)))

# It's time to scan subdirs and prepare mods info.
# ...and to build dependency injection model
include mk/codegen-di.mk

APIS_BUILD := $(call filter_abstract_modules,$(MODS_ENABLE_OBJ))
export LIBS_BUILD := $(call filter_static_modules,$(MODS_ENABLE_OBJ))
export MODS_BUILD := $(filter-out $(LIBS_BUILD),$(MODS_ENABLE_OBJ))

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

__rootfs_image := $(abspath $(ROOTFS_IMAGE))

#XXX
$(OBJ_DIR)/src/fs/ramfs/ramfs_cpio.o : $(__rootfs_image)

ROOTFS_OBJS_BUILD := $(call rootfs_src_to_obj,$(ROOTFS_SRCS_BUILD))

$(ROOTFS_DIR) :
	@$(MKDIR) $@

$(USER_ROOTFS_DIR) :
	@$(MKDIR) $@

$(__rootfs_image): $(ROOTFS_DIR) $(USER_ROOTFS_DIR) $(ROOTFS_OBJS_BUILD)
	@cd $< \
	&& find * -depth -print 2>/dev/null | cpio --quiet -H newc -o -O $@
	@if [ -d $(USER_ROOTFS_DIR) ]; then \
		cd $(USER_ROOTFS_DIR); \
		find * -depth -print 2>/dev/null | cpio --quiet -H newc -o --append -O $@; \
	fi

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

OBJ_SUBDIRS := \
	$(sort $(dir $(OBJS_BUILD) $(LDSS_BUILD)))

include mk/headers.mk

$(OBJS_BUILD) $(LDSS_BUILD): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk

__CMDS = \
  $(patsubst $(abspath $(ROOT_DIR))%.$1,$(OBJ_DIR)%.cmd,$(filter %.$1,$(abspath $(SRCS_BUILD))))

CMDS_C := $(call __CMDS,c)
CMDS_CPP := $(call __CMDS,cpp)
CMDS_S := $(call __CMDS,S)

CMDS := $(CMDS_C) $(CMDS_CPP) $(CMDS_S)

$(CMDS_C) : __FLAGS = $(CFLAGS) $(CPPFLAGS) $(CCFLAGS)
$(CMDS_S) : __FLAGS = $(ASFLAGS) $(CPPFLAGS) $(CCFLAGS)
$(CMDS_CPP) : __FLAGS = $(CXXFLAGS) $(CPPFLAGS) $(CCFLAGS)

$(CMDS) : FLAGS = $(subst ",,$(__FLAGS))

$(CMDS) : %.cmd : %.cmd.tmp ;

.SECONDEXPANSION:
$(CMDS:%.cmd=%.cmd.tmp): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk \
		mk/image.mk $(myfiles_model_mk) | $$(@D)
	@echo '$(FLAGS) -o $(@:%.cmd.tmp=%.o) -c' > $@
	@diff -q $@ $(subst .tmp,,$@) >/dev/null 2>&1 || cp $@ $(subst .tmp,,$@)

$(sort $(dir $(CMDS))) :
	@$(MKDIR) $@

ifndef VERBOSE
ifdef CC_SUPPORTS_@file
CC_RULES = $(CC) @$< $(word 2,$^)
else
CC_RULES = $(CC) `cat $<` $(word 2,$^)
endif
else
CC_RULES = $(CC) $(patsubst -D%,-D"%",$(shell cat $<)) $(word 2,$^)
endif

CPP_RULES = g++ `@$<` $(word 2,$^)

$(OBJ_DIR)/%.o : $(OBJ_DIR)/%.cmd $(ROOT_DIR)/%.c
	$(CC_RULES)

$(OBJ_DIR)/%.o : $(OBJ_DIR)/%.cmd $(ROOT_DIR)/%.cpp
	$(CPP_RULES)


$(OBJ_DIR)/%.o : $(OBJ_DIR)/%.cmd $(ROOT_DIR)/%.S
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

