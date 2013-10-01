
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

ifeq (arm,$(EMBOX_ARCH))
$(error Unsupported architecture: $(EMBOX_ARCH))
else ifeq (x86,$(EMBOX_ARCH))
EMBOX_GCC_TARGET=i386-pc-msdosdjgpp
#i386-pc-elf
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif


AT=

GCC_DIR       = $(ROOT_DIR)/build/gcc
GCC_CMAKE_DIR = $(GCC_DIR)/cmake
GCC_LIBS     += $(GCC_DIR)/$(EMBOX_GCC_TARGET)/lib/libsupc++.a
GCC_LIBS     += $(GCC_DIR)/lib/gcc/$(EMBOX_GCC_TARGET)/4.8.1/libgcc.a
GCC_INCLUDES += $(GCC_DIR)/$(EMBOX_GCC_TARGET)/include/c++/4.8.1
GCC_INCLUDES += $(GCC_DIR)/lib/gcc/$(EMBOX_GCC_TARGET)/4.8.1/include
GCC_FLAGS     = $(GCC_CMAKE_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/gcc/gcc_embox_compat.h
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

EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/gcc/include
EMBOX_IMPORTED_CXXFLAGS += -I$(ROOT_DIR)/build/gcc/include

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
EMBOX_DERIVED_TARGET    += $(EMBOX_GCC_TARGET)

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib

GCC_DERIVED_LIBS = $(foreach lib,$(GCC_LIBS),$(GCC_DIR)/lib/$(notdir $(lib)))

all:

.PHONY: all $(GCC_LIB)

$(GCC_CMAKE_DIR):
	$(AT)mkdir -p $@

$(GCC_FLAGS): | $(GCC_CMAKE_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_TARGET=\"$(EMBOX_DERIVED_TARGET)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(GCC_CMAKE_DIR)/Makefile: CMakeLists.txt $(GCC_FLAGS) | $(GCC_CMAKE_DIR)
	$(AT)cd $(GCC_CMAKE_DIR) && $(shell cat $(GCC_FLAGS)) cmake -DCMAKE_BUILD_TYPE=Debug $(shell pwd)

$(GCC_LIBS): $(GCC_CMAKE_DIR)/Makefile
	$(AT)cd $(GCC_CMAKE_DIR) && $(MAKE) VERBOSE=1 # -j `nproc`

$(GCC_DERIVED_LIBS): $(GCC_LIBS)
	$(AT)cp -u $^ $(GCC_DIR)/lib/
	$(AT)mkdir -p $(GCC_DIR)/include/
	$(AT)cp -u $(GCC_DIR)/src/gcc-build/$(EMBOX_GCC_TARGET)/libstdc++-v3/include/$(EMBOX_GCC_TARGET)/bits/c++config.h $(GCC_DIR)/include/
	$(AT)cp -u $(GCC_DIR)/src/gcc-build/$(EMBOX_GCC_TARGET)/libstdc++-v3/include/$(EMBOX_GCC_TARGET)/bits/os_defines.h $(GCC_DIR)/include/
	$(AT)cp -u $(GCC_DIR)/src/gcc-build/$(EMBOX_GCC_TARGET)/libstdc++-v3/include/$(EMBOX_GCC_TARGET)/bits/cpu_defines.h $(GCC_DIR)/include/
	$(AT)$(foreach inc,$(GCC_INCLUDES),cp -R -u $(inc)/* $(GCC_DIR)/include/ &&) true

all: $(GCC_DERIVED_LIBS)
.NOTPARALLEL:
