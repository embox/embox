
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

CLDC_TARGET_ID=product
ifneq ($(findstring -g,$(EMBOX_CFLAGS)),)
USE_DEBUG=true
DEBUG_SUFFIX=_g
CLDC_TARGET_ID=debug
endif

EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(subst ",',$(filter -D"__impl_x%,$(EMBOX_CPPFLAGS)))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)

EMBOX_IMPORTED_CFLAGS   += -fno-stack-protector
EMBOX_IMPORTED_CXXFLAGS += -fno-stack-protector
EMBOX_IMPORTED_CXXFLAGS += -fpermissive
EMBOX_IMPORTED_CXXFLAGS += -Dtypeof=__typeof__
EMBOX_IMPORTED_CXXFLAGS += -Dexit=embox_phoneme_exit
EMBOX_DERIVED_CFLAGS   += $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS += $(EMBOX_IMPORTED_CXXFLAGS)


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

MIDP_DIR            := $(PHONEME_DIR)/midp
MIDP_OUTPUT_DIR     := $(PHONEME_BUILD_DIR)/midp

.PHONY: checkout javacall pcsl cldc midp amalgamated amalgamated_cldc amalgamated_midp

AT:=

all: amalgamated

checkout: $(TOOLS_DIR) $(JAVACALL_DIR) $(PCSL_DIR) $(CLDC_DIR)

TOOLS_URL = $(word 1,$(shell grep tools $(SPECFILE_TXT)))
$(TOOLS_DIR):
	$(AT)svn co -q $(TOOLS_URL) $@

$(JAVACALL_DIR) $(PCSL_DIR) $(CLDC_DIR):
	$(AT)(cd $(PHONEME_DIR) && tools/svntools/checkout.sh $(abspath $(SPECFILE_TXT)))
	$(AT)patch -p0 -d $(PHONEME_DIR) -N --input=$(abspath $(PATCH_TXT))

javacall: checkout
	export CFLAGS="$(EMBOX_DERIVED_CFLAGS)" && \
	$(AT)$(MAKE) -C $(JAVACALL_DIR)/configuration/stubs/linux \
		USE_DEBUG=$(USE_DEBUG) \
		AT= \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
		JAVACALL_DIR=$(JAVACALL_DIR) \
		TOOLS_DIR=$(TOOLS_DIR) \
		JDK_DIR=$(JDK_DIR)

pcsl: javacall checkout
	$(AT)
	(export CFLAGS="$(EMBOX_DERIVED_CFLAGS)" && \
	$(MAKE) -C $(PCSL_DIR) \
		USE_DEBUG=$(USE_DEBUG) \
		USE_VERBOSE_MAKE=" true" \
		PCSL_PLATFORM=javacall_i386_gcc \
		PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
	)

cldc: pcsl javacall checkout
	$(AT) \
	(export LINK_FLAGS="$(JAVACALL_OUTPUT_DIR)/lib/libjavacall$(DEBUG_SUFFIX).a" && \
	$(MAKE) -C $(CLDC_DIR)/build/javacall_i386_gcc \
		ENABLE_COMPILATION_WARNINGS=true \
		ENABLE_MAP_FILE=false \
		VERBOSE_BUILD=true \
		CXXFLAGS_EXTERNAL_TARGET="$(EMBOX_DERIVED_CXXFLAGS)" \
		JAVACALL_LIB_DIR=$(JAVACALL_OUTPUT_DIR)/lib \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
		PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
		JVMWorkSpace=$(CLDC_DIR) \
		JVMBuildSpace=$(CLDC_OUTPUT_DIR) \
		JDK_DIR=$(JDK_DIR) \
	)

midp: cldc pcsl javacall checkout
	$(AT)export EXTRA_CFLAGS="$(EMBOX_DERIVED_CFLAGS)" && \
	$(MAKE) -C $(MIDP_DIR)/build/javacall \
		TOOLS_DIR=$(TOOLS_DIR) \
		JDK_DIR=$(JDK_DIR) \
		USE_DEBUG=$(USE_DEBUG) \
		USE_VERBOSE_MAKE=true \
		CREATE_MIDP_SHARED_LIB=false \
		LD=true \
		LD_END_GROUP=" -Xlinker --end-group " \
		JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
		PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
		MIDP_OUTPUT_DIR=$(MIDP_OUTPUT_DIR) \
		CLDC_DIST_DIR=$(CLDC_DIST_DIR) \
		JAVACALL_PLATFORM=linux_i386_gcc

AMAL_OUTPUT_DIR := $(PHONEME_BUILD_DIR)/amalgamated
AMAL_OBJ_DIR    := $(AMAL_OUTPUT_DIR)/obj
AMAL_LIB_DIR    := $(AMAL_OUTPUT_DIR)/lib
AMAL_GEN_DIR    := $(AMAL_OUTPUT_DIR)/gen
AMAL_FLAG_DIR   := $(AMAL_OUTPUT_DIR)/_created

CLDC_DIST_DIR=$(CLDC_OUTPUT_DIR)/javacall_i386_gcc/dist

ROM_GENERATOR   = $(CLDC_DIST_DIR)/bin/romgen
JCC_CMD         = $(CLDC_DIST_DIR)/bin/NativesTableGen.jar
CLDC_ZIP        = $(CLDC_DIST_DIR)/lib/cldc_classes.zip
ROM_CONFIG_FILE = $(CLDC_DIST_DIR)/lib/cldc_rom.cfg
ROM_IMAGE       = $(AMAL_GEN_DIR)/ROMImage.cpp
NATIVES_TABLE   = $(AMAL_GEN_DIR)/NativesTable.cpp

JVMSPI_OBJ           = $(CLDC_OUTPUT_DIR)/javacall_i386_gcc/target/$(CLDC_TARGET_ID)/jvmspi.o
JAVAUTIL_CLDC_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javautil_unicode.o
AMAL_CLDC_EXT_OBJS   += $(JVMSPI_OBJ) $(JAVAUTIL_CLDC_OBJS)

JAVAUTIL_MIDP_OBJS   += $(JAVAUTIL_CLDC_OBJS)
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/datagram.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/socket.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/network.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/sos_network.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/permissions.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/annunciator.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/font.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/lifecycle.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javacall_properties.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javacall_config_db.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javacall_db.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javautil_string.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javautil_stdio.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/javautil_printf.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/string.o
AMAL_MIDP_EXT_OBJS   += $(JAVACALL_OUTPUT_DIR)/obj$(DEBUG_SUFFIX)/serial.o
AMAL_MIDP_EXT_OBJS   += $(JAVAUTIL_MIDP_OBJS)

ROM_GEN_FLAGS       += +GenerateGNUCode

ROM_GEN_ARG         += $(ROM_GEN_FLAGS) =HeapCapacity16M
ROM_GEN_ARG         += -romconfig $(ROM_CONFIG_FILE)
ROM_GEN_ARG         += -romincludepath $(CLDC_DIR)/src/vm
ROM_GEN_ARG         += +RewriteROMConstantPool
ROM_GEN_ARG         += +EnableAllROMOptimizations
ifeq ($(ENABLE_JAVA_DEBUGGER), true)
ifeq ($(ENABLE_SYSTEM_CLASSES_DEBUG), true)
  ROM_GEN_ARG       += +MakeROMDebuggable
  JAVAC_DEBUG       =
else
  # For debugging application classes -CompactROMMethodTables
  # is the only optimization which is incompatible with the Java debugger.
  # For debugging Monet bundle the +MakeROMDebuggable is reduced
  # to -CompactROMMethodTables (all the other optimizations are
  # disabled for binary image generator)
  ROM_GEN_ARG       += -CompactROMMethodTables
endif
endif

AMAL_COMMON_SOURCES    += javacall_cldc_file.c
AMAL_COMMON_SOURCES    += javacall_cldc_memory.c
AMAL_COMMON_SOURCES    += javacall_cldc_time.c
AMAL_COMMON_SOURCES    += javacall_cldc_dir.c
AMAL_COMMON_SOURCES    += javacall_cldc_os.c
AMAL_COMMON_SOURCES    += javacall_cldc_logging.c
AMAL_COMMON_SOURCES    += embox_java_compat.c

AMAL_CLDC_SOURCES    += $(AMAL_COMMON_SOURCES)
AMAL_CLDC_SOURCES    += $(ROM_IMAGE)
AMAL_CLDC_SOURCES    += $(NATIVES_TABLE)
AMAL_CLDC_SOURCES    += cldc_standalone.c

AMAL_MIDP_SOURCES    += $(AMAL_COMMON_SOURCES)
AMAL_MIDP_SOURCES    += midp.c
AMAL_MIDP_SOURCES    += javacall_midp_events.c

AMAL_CLDC_OBJECTS    += $(patsubst %,$(AMAL_OBJ_DIR)/%.o, $(basename $(filter %.c %.cpp,$(notdir $(AMAL_CLDC_SOURCES)))))

AMAL_MIDP_OBJECTS    += $(patsubst %,$(AMAL_OBJ_DIR)/%.o, $(basename $(filter %.c %.cpp,$(notdir $(AMAL_MIDP_SOURCES)))))

VPATH           += .
# Directory has to exist if using VPATH :(
$(shell mkdir -p $(AMAL_GEN_DIR))
VPATH           += $(AMAL_GEN_DIR)

AMAL_CLDC_LOCAL_LIB  := $(AMAL_LIB_DIR)/local_cldc.a
AMAL_CLDC_LIB   := $(AMAL_LIB_DIR)/phoneme_cldc.a

AMAL_MIDP_LOCAL_LIB  := $(AMAL_LIB_DIR)/local_midp.a
AMAL_MIDP_LIB   := $(AMAL_LIB_DIR)/phoneme_midp.a

AMAL_INCLUDE_DIRS += $(CLDC_DIST_DIR)/include
AMAL_INCLUDE_DIRS += $(PCSL_OUTPUT_DIR)/javacall_i386/inc
AMAL_INCLUDE_DIRS += $(JAVACALL_OUTPUT_DIR)/inc

AMAL_CPPFLAGS   += $(EMBOX_CPPFLAGS)
AMAL_CPPFLAGS   += -D__int64="long long"
AMAL_CPPFLAGS   += -DROMIZING
AMAL_CPPFLAGS   += -I$(ROOT_DIR)/src/compat/posix/include
AMAL_CPPFLAGS   += $(patsubst %,-I%,$(AMAL_INCLUDE_DIRS))

AMAL_CFLAGS     += $(EMBOX_CFLAGS)
AMAL_CFLAGS     := $(filter-out -Wstrict-prototypes,$(AMAL_CFLAGS))
AMAL_CFLAGS     := $(filter-out -Wundef,$(AMAL_CFLAGS))

AMAL_CXXFLAGS   += $(filter-out -Werror,$(EMBOX_CXXFLAGS))

PHONEME_CLDC_LIBS += $(CLDC_DIST_DIR)/lib/libcldc_vm$(DEBUG_SUFFIX).a
PHONEME_CLDC_LIBS += $(PCSL_OUTPUT_DIR)/javacall_i386/lib/libpcsl_memory.a
PHONEME_CLDC_LIBS += $(PCSL_OUTPUT_DIR)/javacall_i386/lib/libpcsl_print.a
PHONEME_CLDC_LIBS += $(PCSL_OUTPUT_DIR)/javacall_i386/lib/libpcsl_network.a
PHONEME_CLDC_LIBS += $(PCSL_OUTPUT_DIR)/javacall_i386/lib/libpcsl_string.a
PHONEME_CLDC_LIBS += $(PCSL_OUTPUT_DIR)/javacall_i386/lib/libpcsl_file.a

#PHONEME_MIDP_LIBS += $(PHONEME_CLDC_LIBS) #these are already in libjvm.a
PHONEME_MIDP_LIBS += $(MIDP_OUTPUT_DIR)/bin/i386/libjvm$(DEBUG_SUFFIX).a

$(AMAL_OBJ_DIR) $(AMAL_LIB_DIR) $(AMAL_GEN_DIR) $(AMAL_FLAG_DIR):
	$(AT)mkdir -p $@

$(AMAL_FLAG_DIR): | $(AMAL_OBJ_DIR) $(AMAL_LIB_DIR) $(AMAL_GEN_DIR)

$(ROM_GENERATOR) $(CLDC_ZIP) $(ROM_CONFIG_FILE): cldc

$(ROM_IMAGE): $(ROM_GENERATOR) $(CLDC_ZIP) $(ROM_CONFIG_FILE)
	$(AT)cd $(dir $@) && \
		$(ROM_GENERATOR) -cp $(CLDC_ZIP) $(ROM_GEN_ARG) -romize

$(NATIVES_TABLE): $(CLDC_ZIP) $(JCC_CMD)
	$(AT)cd $(dir $@) && \
		java -jar $(JCC_CMD) -writer CLDC_HI_Natives -o $@ $(CLDC_ZIP)

$(AMAL_CLDC_OBJECTS): $(AMAL_INCLUDE_DIRS) $(AMAL_FLAG_DIR)

$(AMAL_INCLUDE_DIRS): javacall pcsl cldc

$(AMAL_CLDC_EXT_OBJS) $(AMAL_MIDP_EXT_OBJS): javacall pcsl cldc

$(PHONEME_CLDC_LIBS): pcsl cldc

$(PHONEME_MIDP_LIBS): pcsl cldc midp

$(AMAL_CLDC_LOCAL_LIB): $(AMAL_CLDC_EXT_OBJS) $(AMAL_CLDC_OBJECTS)
	$(AT)ar -rcs $@ $^

$(AMAL_CLDC_LIB): $(AMAL_CLDC_LOCAL_LIB) $(PHONEME_CLDC_LIBS)
	-$(AT)rm *.o
	$(AT)$(patsubst %,ar -x % &&,$?) true
	$(AT)ar -rcs $@ *.o
	$(AT)rm *.o
	$(AT)ranlib $@

$(AMAL_MIDP_LOCAL_LIB): $(AMAL_MIDP_EXT_OBJS) $(AMAL_MIDP_OBJECTS)
	$(AT)ar -rcs $@ $^

$(AMAL_MIDP_LIB): $(AMAL_MIDP_LOCAL_LIB) $(PHONEME_MIDP_LIBS)
	-$(AT)rm *.o
	$(AT)$(patsubst %,ar -x % &&,$?) true
	$(AT)ar -rcs $@ *.o
	$(AT)rm *.o
	$(AT)ranlib $@

$(AMAL_OBJ_DIR)/%.o: %.cpp
	$(AT)cd ../../ && g++ $(AMAL_CPPFLAGS) $(AMAL_CXXFLAGS) -o $@ -c $<

$(AMAL_OBJ_DIR)/%.o: %.c
	$(AT)cd ../../ && gcc $(AMAL_CPPFLAGS) $(AMAL_CFLAGS) -o $@ -c $(abspath $<)

PHONEME_CLDC_PREVERIFY = $(CLDC_DIST_DIR)/bin/preverify
HELLOWORLD_DIR = $(AMAL_OUTPUT_DIR)/bin/hello
HELLOWORLD_APP = $(HELLOWORLD_DIR)/dist/hello.jar

$(PHONEME_CLDC_PREVERIFY): cldc

$(HELLOWORLD_APP): $(PHONEME_CLDC_PREVERIFY) $(CLDC_ZIP) $(shell find hello -type f)
	$(AT)cd hello && ant \
		-Dpreverify.exec=$(PHONEME_CLDC_PREVERIFY) \
		-Dclasses.zip=$(CLDC_ZIP) \
		-Dappbuild.dir=$(HELLOWORLD_DIR)

amalgamated_cldc: $(AMAL_CLDC_LIB) $(HELLOWORLD_APP)

amalgamated_midp: $(AMAL_MIDP_LIB)

amalgamated: amalgamated_cldc amalgamated_midp

.NOTPARALLEL:
