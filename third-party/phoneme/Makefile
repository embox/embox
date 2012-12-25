
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

ifneq ($(findstring -g,$(EMBOX_CFLAGS)),)
USE_DEBUG=true
DEBUG_SUFFIX=_g
endif

PHONEME_DIR := $(ROOT_DIR)/build/phoneme
PHONEME_BUILD_DIR := $(PHONEME_DIR)/build
SPECFILE_TXT = phoneme_specfile.txt
PATCH_TXT = phoneme_patch.txt

TOOLS_DIR           := $(PHONEME_DIR)/tools
JAVACALL_DIR        := $(PHONEME_DIR)/javacall
JAVACALL_OUTPUT_DIR := $(PHONEME_BUILD_DIR)/javacall
JDK_DIR             := /usr/lib/jvm/java-6-openjdk-i386

PCSL_DIR            := $(PHONEME_DIR)/pcsl
PCSL_OUTPUT_DIR     := $(PHONEME_BUILD_DIR)/pcsl

CLDC_DIR            := $(PHONEME_DIR)/cldc
CLDC_OUTPUT_DIR     := $(PHONEME_BUILD_DIR)/cldc

.PHONY: checkout javacall pcsl cldc

AT:=

all: cldc

checkout: $(TOOLS_DIR) $(JAVACALL_DIR) $(PCSL_DIR) $(CLDC_DIR)

TOOLS_URL = $(word 1,$(shell grep tools $(SPECFILE_TXT)))
$(TOOLS_DIR):
	$(AT)svn co -q $(TOOLS_URL) $@

$(JAVACALL_DIR) $(PCSL_DIR) $(CLDC_DIR):
	$(AT)(cd $(PHONEME_DIR) && tools/svntools/checkout.sh $(abspath $(SPECFILE_TXT)))
	$(AT)patch -p0 -d $(PHONEME_DIR) -N --input=$(abspath $(PATCH_TXT))

javacall: checkout
	$(AT)$(MAKE) -C $(JAVACALL_DIR)/configuration/stubs/linux \
		USE_DEBUG=$(USE_DEBUG) \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
		JAVACALL_DIR=$(JAVACALL_DIR) \
		TOOLS_DIR=$(TOOLS_DIR) \
		JDK_DIR=$(JDK_DIR)

pcsl: javacall checkout
	$(AT)$(MAKE) -C $(PCSL_DIR) \
		USE_DEBUG=$(USE_DEBUG) \
		PCSL_PLATFORM=javacall_i386_gcc \
		PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR)

cldc: pcsl javacall checkout
	$(AT) \
	(export LINK_FLAGS=$(JAVACALL_OUTPUT_DIR)/lib/libjavacall$(DEBUG_SUFFIX).a \
			CPP_DEF_FLAGS=-fno-stack-protector && \
	$(MAKE) -C $(CLDC_DIR)/build/javacall_i386_gcc \
		ENABLE_COMPILATION_WARNINGS=true \
		ENABLE_MAP_FILE=false \
		VERBOSE_BUILD=true \
		JAVACALL_LIB_DIR=$(JAVACALL_OUTPUT_DIR)/lib \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
		PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
		JVMWorkSpace=$(CLDC_DIR) \
		JVMBuildSpace=$(CLDC_OUTPUT_DIR) \
		JDK_DIR=$(JDK_DIR) \
	)

