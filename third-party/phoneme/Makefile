
PKG_NAME := phoneme
PKG_VER  := trunk

PKG_SOURCES := https://googledrive.com/host/0B7g4WwjaJX23ZExQU2hHTzYyRm8/phoneme-trunk.tar.gz
PKG_MD5     := b552aa86b2832d00563636330ec64299

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

JAVACALL_DIR := $(BUILD_DIR)/$(PKG_NAME)/javacall
JAVACALL_OUTPUT_DIR := $(BUILD_DIR)/$(PKG_NAME)/build/javacall
PCSL_OUTPUT_DIR := $(BUILD_DIR)/$(PKG_NAME)/build/pcsl
CLDC_DIR := $(BUILD_DIR)/$(PKG_NAME)/cldc
CLDC_OUTPUT_DIR := $(BUILD_DIR)/$(PKG_NAME)/build/cldc
CLDC_DIST_DIR := $(CLDC_OUTPUT_DIR)/javacall_i386_gcc/dist
TOOLS_DIR := $(BUILD_DIR)/$(PKG_NAME)/tools
JDK_DIR = $(shell readlink -f `which javac` | xargs dirname)/..

cppflags := $(filter -I%,$(EMBOX_CPPFLAGS))
cppflags += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
cppflags += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
cppflags += $(subst \",\',$(filter -D\"__impl_x%,$(EMBOX_CPPFLAGS)))
cflags   := -fno-stack-protector
cflags   += $(filter -m32,$(EMBOX_CFLAGS))
cxxflags := -fno-stack-protector
cxxflags += $(filter -m32,$(EMBOX_CXXFLAGS))
cxxflags += -fpermissive
cxxflags += -Dtypeof=__typeof__
$(warning $(cppflags))

amal_cppflags := $(EMBOX_CPPFLAGS)
amal_cppflags += -DROMIZING
amal_cppflags += -I$(CLDC_DIST_DIR)/include
amal_cppflags += -I$(PCSL_OUTPUT_DIR)/javacall_i386/inc
amal_cppflags += -I$(JAVACALL_OUTPUT_DIR)/inc
amal_cflags   := $(EMBOX_CFLAGS)
amal_cflags   := $(filter-out -Wstrict-prototypes,$(cflags))
amal_cflags   := $(filter-out -Wundef,$(cflags))
amal_cxxflags := $(EMBOX_CXXFLAGS)
amal_cxxflags := $(filter-out -Werror,$(cxxflags))
$(warning $(amal_cppflags))

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME) && ( \
		make -C javacall/configuration/stubs/linux MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true AT= \
			JAVACALL_DIR=$(JAVACALL_DIR) JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			TOOLS_DIR=$(TOOLS_DIR) \
			JDK_DIR=$(JDK_DIR) \
			CFLAGS='$(cppflags) $(cflags)' \
	)
	cd $(BUILD_DIR)/$(PKG_NAME) && ( \
		make -C pcsl MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true USE_VERBOSE_MAKE=" true" \
			PCSL_PLATFORM=javacall_i386_gcc \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			CFLAGS='$(cppflags) $(cflags)' \
	)
	cd $(BUILD_DIR)/$(PKG_NAME) && ( \
		make -C cldc/build/javacall_i386_gcc MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			ENABLE_COMPILATION_WARNINGS=true \
			ENABLE_MAP_FILE=false \
			VERBOSE_BUILD=true \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JVMWorkSpace=$(CLDC_DIR) JVMBuildSpace=$(CLDC_OUTPUT_DIR) \
			JDK_DIR=$(JDK_DIR) \
			CXXFLAGS_EXTERNAL_TARGET='$(cppflags) $(cxxflags)' \
			LINK_FLAGS='$(JAVACALL_OUTPUT_DIR)/lib/libjavacall_g.a' \
	)
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/obj && \
		cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/obj && ( \
			gcc $(amal_cppflags) $(amal_cflags) -c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_file.c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_memory.c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_time.c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_dir.c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_os.c \
				$(ROOT_DIR)/third-party/phoneme/javacall_cldc_logging.c \
				$(ROOT_DIR)/third-party/phoneme/embox_java_compat.c \
		)
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/gen && \
		cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/gen && ( \
			$(CLDC_DIST_DIR)/bin/romgen -cp $(CLDC_DIST_DIR)/lib/cldc_classes.zip \
				+GenerateGNUCode =HeapCapacity16M \
				-romconfig $(CLDC_DIST_DIR)/lib/cldc_rom.cfg \
				-romincludepath $(CLDC_DIR)/src/vm \
				+RewriteROMConstantPool +EnableAllROMOptimizations -romize \
		)
	cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/obj && ( \
		g++ $(amal_cppflags) $(amal_cxxflags) -c \
			$(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/gen/ROMImage.cpp \
	)
	cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/gen && ( \
		java -jar $(CLDC_DIST_DIR)/bin/NativesTableGen.jar \
			-writer CLDC_HI_Natives -o NativesTable.cpp $(CLDC_DIST_DIR)/lib/cldc_classes.zip \
	)
	cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/obj && ( \
		g++ $(amal_cppflags) $(amal_cxxflags) -c \
			$(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/gen/NativesTable.cpp \
	)
	cd $(BUILD_DIR)/$(PKG_NAME)/build && \
		mkdir -p amalgamated/lib && ( \
			ar -rcs amalgamated/lib/local_cldc.a \
				cldc/javacall_i386_gcc/target/debug/jvmspi.o \
				javacall/obj_g/javautil_unicode.o \
				amalgamated/obj/javacall_cldc_file.o \
				amalgamated/obj/javacall_cldc_memory.o \
				amalgamated/obj/javacall_cldc_time.o \
				amalgamated/obj/javacall_cldc_dir.o \
				amalgamated/obj/javacall_cldc_os.o \
				amalgamated/obj/javacall_cldc_logging.o \
				amalgamated/obj/embox_java_compat.o \
				amalgamated/obj/ROMImage.o \
				amalgamated/obj/NativesTable.o \
		)
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/tmp && \
		cd $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/tmp && ( \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/lib/local_cldc.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/cldc/javacall_i386_gcc/dist/lib/libcldc_vm_g.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/pcsl/javacall_i386/lib/libpcsl_memory.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/pcsl/javacall_i386/lib/libpcsl_print.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/pcsl/javacall_i386/lib/libpcsl_network.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/pcsl/javacall_i386/lib/libpcsl_string.a && \
			ar -x $(BUILD_DIR)/$(PKG_NAME)/build/pcsl/javacall_i386/lib/libpcsl_file.a && \
			ar -rcs $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/lib/phoneme_cldc.a *.o && \
			rm -r $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/tmp && \
			ranlib $(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/lib/phoneme_cldc.a \
		)
	cd $(ROOT_DIR)/third-party/phoneme/hello && ( \
		ant -Dpreverify.exec=$(CLDC_DIST_DIR)/bin/preverify \
			-Dclasses.zip=$(CLDC_DIST_DIR)/lib/cldc_classes.zip \
			-Dappbuild.dir=$(BUILD_DIR)/$(PKG_NAME)/build/amalgamated/bin/hello \
	)
	touch $@

$(INSTALL) :
	touch $@
