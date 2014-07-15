ifeq (arm,$(EMBOX_ARCH))
EMBOX_E2FSPROGS_TARGET=arm-none-eabi
else ifeq (x86,$(EMBOX_ARCH))
EMBOX_E2FSPROGS_TARGET=i386-pc-elf
else ifeq (microblaze,$(EMBOX_ARCH))
EMBOX_E2FSPROGS_TARGET=microblaze-elf
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

EMBOX_TARGET_CC =$(shell which $(EMBOX_CROSS_COMPILE)gcc)
EMBOX_TARGET_CXX=$(shell which $(EMBOX_CROSS_COMPILE)g++)

ifeq (,$(EMBOX_TARGET_CC))
$(error Cannot find C compiler: $(EMBOX_CROSS_COMPILE)gcc)
endif
ifeq (,$(EMBOX_TARGET_CXX))
$(error Cannot find C++ compiler: $(EMBOX_CROSS_COMPILE)g++)
endif



AT=

E2FSPROGS_DIR   = $(BUILD_DIR)
E2FSPROGS_FLAGS = $(E2FSPROGS_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
#EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/e2fsprogs/e2fsprogs_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS   += $(filter -I%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -std=gnu99,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -g,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -ffixed-r31,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-xl-soft-mul,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -ffixed-r31,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-xl-soft-mul,$(EMBOX_CXXFLAGS))

#EMBOX_IMPORTED_CFLAGS   += -Wl,--relocatable
#EMBOX_IMPORTED_CXXFLAGS += -Wl,--relocatable

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS)
EMBOX_DERIVED_TARGET     = $(EMBOX_E2FSPROGS_TARGET)
EMBOX_DERIVED_CC         = $(EMBOX_TARGET_CC)
EMBOX_DERIVED_CXX        = $(EMBOX_TARGET_CXX)

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib
EMBOX_DERIVED_LDFLAGS   += $(foreach i,$(filter -m elf_i386,$(EMBOX_LDFLAGS)),-Wl,$i)

all:

.PHONY: all $(E2FSPROGS_LIB)

$(E2FSPROGS_DIR):
	$(AT)mkdir -p $@

$(E2FSPROGS_FLAGS): | $(E2FSPROGS_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_TARGET=\"$(EMBOX_DERIVED_TARGET)\" >> $@
	$(AT)echo EMBOX_DERIVED_CC=\"$(EMBOX_DERIVED_CC)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXX=\"$(EMBOX_DERIVED_CC)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(E2FSPROGS_DIR)/Makefile: CMakeLists.txt $(E2FSPROGS_FLAGS) | $(E2FSPROGS_DIR)
	$(AT)cd $(E2FSPROGS_DIR) && $(shell cat $(E2FSPROGS_FLAGS)) cmake -DCMAKE_BUILD_TYPE=Debug $(shell pwd)

all: $(E2FSPROGS_DIR)/Makefile
	$(AT)cd $(E2FSPROGS_DIR) && $(MAKE) VERBOSE=1 # -j `nproc`

.NOTPARALLEL:
