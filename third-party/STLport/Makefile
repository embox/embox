
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

AT=

STLPORT_DIR       = $(ROOT_DIR)/build/stlport
STLPORT_CMAKE_DIR = $(STLPORT_DIR)/cmake
STLPORT_LIB       = $(ROOT_DIR)/embox-lib/libstlportg.a
STLPORT_FLAGS     = $(STLPORT_CMAKE_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I../../stlport
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += -include $(abspath .)/stlport_embox_compat.h
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

EMBOX_IMPORTED_CFLAGS   += -I$(abspath .)/include
EMBOX_IMPORTED_CXXFLAGS += -I$(abspath .)/include

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif
# no wonder the following doesn't work
# EMBOX_IMPORTED_MAKEFLAGS += $(filter --jobserver-fds=%,$(EMBOX_MAKEFLAGS))

EMBOX_DERIVED_CFLAGS    += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  += $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS += $(EMBOX_IMPORTED_MAKEFLAGS)


all:

.PHONY: all $(STLPORT_LIB)

$(STLPORT_CMAKE_DIR):
	$(AT)mkdir -p $@

$(STLPORT_FLAGS): | $(STLPORT_CMAKE_DIR)
	-$(AT)rm $@
	$(AT)echo EMBOX_DERIVED_CFLAGS=\"$(EMBOX_DERIVED_CFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_CXXFLAGS=\"$(EMBOX_DERIVED_CXXFLAGS)\" >> $@
	$(AT)echo EMBOX_DERIVED_MAKEFLAGS=\"$(EMBOX_DERIVED_MAKEFLAGS)\" >> $@
	$(AT)echo EMBOX_ROOT=\"$(ROOT_DIR)\" >> $@

$(STLPORT_CMAKE_DIR)/Makefile: CMakeLists.txt $(STLPORT_FLAGS) | $(STLPORT_CMAKE_DIR)
	$(AT)cd $(STLPORT_CMAKE_DIR) && $(shell cat $(STLPORT_FLAGS)) cmake $(shell pwd)

$(STLPORT_LIB): $(STLPORT_CMAKE_DIR)/Makefile
	$(AT)cd $(STLPORT_CMAKE_DIR) && $(MAKE) # -j `nproc`

all: $(STLPORT_LIB)

