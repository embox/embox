
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

ifeq (arm,$(EMBOX_ARCH))
EMBOX_QPID_ARCH=arm
else ifeq (x86,$(EMBOX_ARCH))
EMBOX_QPID_ARCH=x86
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

QPID_DIR       = $(ROOT_DIR)/build/qpid
QPID_CMAKE_DIR = $(QPID_DIR)/cmake
QPID_LIB      += $(QPID_DIR)/lib/libqpid.a
QPID_FLAGS     = $(QPID_CMAKE_DIR)/flags

#EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/build/boost/include
EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/build/stlport/include/stlport
EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/third-party/STLport/include
EMBOX_IMPORTED_CPPFLAGS += -I$(ROOT_DIR)/build/e2fsprogs/include
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/STLport/stlport_embox_compat.h

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/qpid/qpid_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -U%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS := $(filter-out -D"__impl_x%,$(EMBOX_IMPORTED_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)

EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -std=gnu99,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -g,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

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
EMBOX_DERIVED_ARCH       = $(EMBOX_QPID_ARCH)
EMBOX_DERIVED_CC         = $(EMBOX_TARGET_CC)
EMBOX_DERIVED_CXX        = $(EMBOX_TARGET_CXX)

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib

all:

.PHONY: all $(QPID_LIB)

$(QPID_CMAKE_DIR):
	$(AT)mkdir -p $@

$(QPID_FLAGS): | $(QPID_CMAKE_DIR)
	-$(AT)rm $@
	$(AT)echo CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo BOOST_ROOT=\"$(ROOT_DIR)/build/boost\" >> $@
	$(AT)echo EMBOX_DERIVED_ARCH=\"$(EMBOX_DERIVED_ARCH)\" >> $@
	$(AT)echo EMBOX_DERIVED_CC=\"$(EMBOX_DERIVED_CC)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXX=\"$(EMBOX_DERIVED_CXX)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(QPID_CMAKE_DIR)/Makefile: CMakeLists.txt $(QPID_FLAGS) | $(QPID_CMAKE_DIR)
	$(AT)cd $(QPID_CMAKE_DIR) && $(shell cat $(QPID_FLAGS)) CFLAGS= CXXFLAGS= cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CROSSCOMPILING=1 $(shell pwd)

$(QPID_LIB): $(QPID_CMAKE_DIR)/Makefile $(QPID_FLAGS)
	$(AT)cd $(QPID_CMAKE_DIR) && $(shell cat $(QPID_FLAGS)) $(MAKE) VERBOSE=1 # -j `nproc`

all: $(QPID_LIB)
.NOTPARALLEL:
