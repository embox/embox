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
	@mkdir -p $(OBJ_SUBDIRS)

#TODO rootfs make target is bad stily
rootfs_prepare: ;
#	@mkdir -p $(BUILD_DIR)/rootfs
#	@cp $(__ROOTFS_SRCS) $(BUILD_DIR)/rootfs/

$(ROOTFS_IMAGE):
	@find $(ROOTFS_DIR)/ -depth -print | cpio -H newc -o > $@

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

ifneq ($(or $(call >,$(CC_VERSION_MAJOR),4), \
       $(and $(call  ==,$(CC_VERSION_MAJOR),4), \
             $(call >=,$(CC_VERSION_MINOR),2))),)
CC_SUPPORTS_@file := 1
endif

#ifeq ($(C),1)
#CHECK   = sparse
#REAL_CC = $(CC)
#CC     := cgcc
#endif

include mk/flags.mk

SRC_TO_OBJ = \
	$(call filter-patsubst, \
		$(abspath $(ROOT_DIR))%.c $(abspath $(ROOT_DIR))%.S,$(OBJ_DIR)%.o, \
		$(filter-out %.lds.S,$(abspath $1)))

LIB_FILE = \
	$(foreach 1,$1,$(LIB_DIR)/$(get $1.qualified_name).a)

# It's time to scan subdirs and prepare mods info.
#include $(MK_DIR)/embuild.mk
#include mk/mybuild/read.mk
# ...and to build dependency injection model
include $(MK_DIR)/codegen-di.mk

# param $1 is module obj
module_get_objects = \
	$(call SRC_TO_OBJ,$(module_get_sources))
module_get_files = \
	$(foreach s,$(get $1.sources),$(get s->fullname))
module_get_sources = \
	$(filter %.c %.S,$(module_get_files))
module_get_headers = \
	$(filter %.h,$(module_get_files))

filter_abstract_modules = \
	$(strip $(foreach m,$1,$(if $(filter abstract,$(get m->modifiers)),$m)))
filter_static_modules = \
	$(strip $(foreach m,$1,$(if $(filter static,$(get m->modifiers)),$m)))

$(def_all)

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

ifdef LDSS_BUILD
LD_SINGLE_T_OPTION := \
	$(if $(value LD_SINGLE_T_OPTION),$(filter-out n,$(LD_SINGLE_T_OPTION)))
ifndef LD_SINGLE_T_OPTION
override LDFLAGS += $(LDSS_BUILD:%=-T %)
else
override LDFLAGS += -T $(LDSS_BUILD)
endif
endif

# 1. Module.
define define_mod_obj_rules
	${eval \
		$(with $1,$(call module_get_objects,$1),
			$(if $(get $1.flags),
				$2 : override CCFLAGS += \
						$(for s<-$(get $1.flags),$(get s->value))
				$(\n))
			$2 : override CPPFLAGS += \
					-D__EMBUILD_MOD__='$(subst .,__,$(get $1.qualified_name))'
		)
	}
endef

# 1. Library module.
define define_lib_rules
	${eval \
		$(call LIB_FILE,$1) : $(call module_get_objects,$1)$(\n)
			$(\t)$(AR) $(ARFLAGS) $$@ $$(^:%= \$$(\n)	%)
	}
endef

$(def_all)

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

$(CMDS): $(AUTOCONF_DIR)/config.h $(AUTOCONF_DIR)/build.mk $(MK_DIR)/image.mk

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

