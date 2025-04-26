
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

AT=

ZEROMQ_CMAKE_DIR = $(MOD_BUILD_DIR)/cmake
ZEROMQ_LIB       = $(MOD_BUILD_DIR)/lib/libzeromq.a
ZEROMQ_FLAGS     = $(ZEROMQ_CMAKE_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I$(EXTERNAL_BUILD_DIR)/third_party/STLport/libstlportg/install/include/stlport
EMBOX_IMPORTED_CPPFLAGS += -I$(abspath .)/include
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/STLport/stlport_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += -include $(ROOT_DIR)/third-party/zeromq/zeromq_embox_compat.h
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)

EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_CFLAGS   += -I$(ROOT_DIR)/build/zeromq/include
EMBOX_IMPORTED_CXXFLAGS += -I$(ROOT_DIR)/build/zeromq/include

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

EMBOX_DERIVED_LDFLAGS   += -Wl,--relocatable -nostdlib
EMBOX_DERIVED_LDFLAGS   += -L$(MOD_BUILD_DIR)/lib
all:

.PHONY: all $(ZEROMQ_LIB)

$(ZEROMQ_CMAKE_DIR):
	$(AT)mkdir -p $@

$(ZEROMQ_FLAGS): | $(ZEROMQ_CMAKE_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_LDFLAGS=\"$(EMBOX_DERIVED_LDFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(ZEROMQ_CMAKE_DIR)/Makefile: CMakeLists.txt $(ZEROMQ_FLAGS) | $(ZEROMQ_CMAKE_DIR)
	$(AT)cd $(ZEROMQ_CMAKE_DIR) && $(shell cat $(ZEROMQ_FLAGS)) cmake -DCMAKE_BUILD_TYPE=Debug $(shell pwd)

$(ZEROMQ_LIB): $(ZEROMQ_CMAKE_DIR)/Makefile
	$(AT)cd $(ZEROMQ_CMAKE_DIR) && $(MAKE) VERBOSE=1 # -j `nproc`

all: $(ZEROMQ_LIB)
