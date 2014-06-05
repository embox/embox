
include $(EXTBLD_LIB)

JAVACALL_OUTPUT_DIR := $(BUILD_DIR)/../javacall
PCSL_OUTPUT_DIR := $(BUILD_DIR)/../pcsl
CLDC_DIR := $(BUILD_DIR)/src
CLDC_OUTPUT_DIR := $(BUILD_DIR)
CLDC_DIST_DIR := $(CLDC_OUTPUT_DIR)/javacall_i386_gcc/dist
JDK_DIR := $(shell readlink -f `which javac` | xargs dirname)/..

cppflags := $(filter -I%,$(EMBOX_CPPFLAGS))
cppflags += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
cppflags += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
cppflags += $(subst \",\',$(filter -D\"__impl_x%,$(EMBOX_CPPFLAGS)))
cxxflags := -fno-stack-protector
cxxflags += $(filter -m32,$(EMBOX_CXXFLAGS))
cxxflags += -fpermissive
cxxflags += -Dtypeof=__typeof__

amal_cppflags := $(EMBOX_CPPFLAGS)
amal_cppflags += -DROMIZING
amal_cppflags += -I$(CLDC_DIST_DIR)/include
amal_cppflags += -I$(PCSL_OUTPUT_DIR)/javacall_i386/inc
amal_cppflags += -I$(JAVACALL_OUTPUT_DIR)/inc
amal_cxxflags := $(EMBOX_CXXFLAGS)
amal_cxxflags := $(filter-out -Werror,$(amal_cxxflags))

$(CONFIGURE) :
	cd $(BUILD_DIR) && ( \
		ln -s ../src/phoneme/cldc src \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR) && ( \
		make -C src/build/javacall_i386_gcc MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			ENABLE_COMPILATION_WARNINGS=true \
			ENABLE_MAP_FILE=false \
			VERBOSE_BUILD=true \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JVMWorkSpace=$(CLDC_DIR) JVMBuildSpace=$(CLDC_OUTPUT_DIR) \
			JDK_DIR=$(JDK_DIR) \
			CXXFLAGS_EXTERNAL_TARGET='$(cppflags) $(cxxflags)' \
	)
	mkdir -p $(BUILD_DIR)/gen && cd $(BUILD_DIR)/gen && ( \
		$(CLDC_DIST_DIR)/bin/romgen -cp $(CLDC_DIST_DIR)/lib/cldc_classes.zip \
			+GenerateGNUCode =HeapCapacity16M \
			-romconfig $(CLDC_DIST_DIR)/lib/cldc_rom.cfg \
			-romincludepath $(CLDC_DIR)/src/vm \
			+RewriteROMConstantPool +EnableAllROMOptimizations -romize && \
		java -jar $(CLDC_DIST_DIR)/bin/NativesTableGen.jar \
			-writer CLDC_HI_Natives -o NativesTable.cpp $(CLDC_DIST_DIR)/lib/cldc_classes.zip \
	)
	# TODO remove this
	cd $(BUILD_DIR)/gen && ( \
			g++ $(amal_cppflags) $(amal_cxxflags) -c ROMImage.cpp && \
			g++ $(amal_cppflags) $(amal_cxxflags) -c NativesTable.cpp \
	)
	touch $@

$(INSTALL) :
	ln -s $(CLDC_DIST_DIR)/include $(PKG_INSTALL_DIR)/include
	ln -s $(CLDC_DIST_DIR)/lib/libcldc_vm_g.a $(PKG_INSTALL_DIR)
	ln -s $(CLDC_OUTPUT_DIR)/javacall_i386_gcc/target/debug/jvmspi.o $(PKG_INSTALL_DIR)
	# For build hello.jar
	ln -s $(CLDC_DIST_DIR)/bin/preverify $(PKG_INSTALL_DIR)
	ln -s $(CLDC_DIST_DIR)/lib/cldc_classes.zip $(PKG_INSTALL_DIR)
	touch $@
