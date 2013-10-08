
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

ifeq (arm,$(EMBOX_ARCH))
$(error Unsupported architecture: $(EMBOX_ARCH))
else ifeq (x86,$(EMBOX_ARCH))
#EMBOX_QPID_TARGET=i386-pc-elf
#i386-pc-elf
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif


AT=

QPID_DIR       = $(ROOT_DIR)/build/qpid
QPID_CMAKE_DIR = $(QPID_DIR)/cmake
QPID_LIBS     += $(QPID_DIR)/$(EMBOX_QPID_TARGET)/lib/libsupc++.a
QPID_LIBS     += $(QPID_DIR)/lib/qpid/$(EMBOX_QPID_TARGET)/4.8.1/libqpid.a
QPID_INCLUDES += $(QPID_DIR)/$(EMBOX_QPID_TARGET)/include/c++/4.8.1
QPID_INCLUDES += $(QPID_DIR)/lib/qpid/$(EMBOX_QPID_TARGET)/4.8.1/include
QPID_FLAGS     = $(QPID_CMAKE_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/qpid/qpid_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))

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
#EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
#EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
#EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/qpid/include
EMBOX_IMPORTED_CXXFLAGS += -I$(ROOT_DIR)/build/qpid/include

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
#EMBOX_DERIVED_TARGET    += $(EMBOX_QPID_TARGET)

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib

all:

.PHONY: all $(QPID_LIB)

$(QPID_CMAKE_DIR):
	$(AT)mkdir -p $@

$(QPID_FLAGS): | $(QPID_CMAKE_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	#$(AT)echo EMBOX_DERIVED_TARGET=\"$(EMBOX_DERIVED_TARGET)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(QPID_CMAKE_DIR)/Makefile: CMakeLists.txt $(QPID_FLAGS) | $(QPID_CMAKE_DIR)
	$(AT)cd $(QPID_CMAKE_DIR) && $(shell cat $(QPID_FLAGS)) cmake -DCMAKE_BUILD_TYPE=Debug $(shell pwd)

$(QPID_LIBS): $(QPID_CMAKE_DIR)/Makefile
	$(AT)cd $(QPID_CMAKE_DIR) && $(MAKE) VERBOSE=1 # -j `nproc`

all: $(QPID_LIBS)
.NOTPARALLEL:
