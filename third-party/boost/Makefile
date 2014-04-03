ifeq (arm,$(EMBOX_ARCH))
EMBOX_BOOST_ARCH=arm
else ifeq (x86,$(EMBOX_ARCH))
EMBOX_BOOST_ARCH=x86
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

BOOST_DIR   = $(BUILD_DIR)
BOOST_FLAGS = $(BOOST_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(abspath .)/boost_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -U%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS := $(filter-out -D"__impl_x%,$(EMBOX_IMPORTED_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS)
EMBOX_DERIVED_ARCH       = $(EMBOX_BOOST_ARCH)
EMBOX_DERIVED_CC         = $(EMBOX_TARGET_CC)
EMBOX_DERIVED_CXX        = $(EMBOX_TARGET_CXX)

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib -m32
all:

.PHONY: all

$(BOOST_DIR):
	$(AT)mkdir -p $@

$(BOOST_FLAGS): | $(BOOST_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_ARCH=\"$(EMBOX_DERIVED_ARCH)\" >> $@
	$(AT)echo EMBOX_DERIVED_CC=\"$(EMBOX_DERIVED_CC)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXX=\"$(EMBOX_DERIVED_CXX)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(BOOST_DIR)/Makefile: CMakeLists.txt $(BOOST_FLAGS) | $(BOOST_DIR)
	$(AT)cd $(BOOST_DIR) && $(shell cat $(BOOST_FLAGS)) cmake $(shell pwd)

all: $(BOOST_DIR)/Makefile
	$(AT)cd $(BOOST_DIR) && $(MAKE) # -j `nproc`
