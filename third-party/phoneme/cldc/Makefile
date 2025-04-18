
ifeq (arm,$(EMBOX_ARCH))
CLDC_TEMPLATE := javacall_linux_arm
ROMIMAGE_EXTRA_CPPFLAGS := -DMSW_FIRST_FOR_DOUBLE
else ifeq (x86,$(EMBOX_ARCH))
CLDC_TEMPLATE := javacall_i386_gcc
ROMIMAGE_EXTRA_CPPFLAGS :=
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

JAVACALL_OUTPUT_DIR := $(BUILD_DIR)/../javacall
PCSL_OUTPUT_DIR := $(BUILD_DIR)/../pcsl
CLDC_DIR := $(BUILD_DIR)/src
CLDC_OUTPUT_DIR := $(BUILD_DIR)
CLDC_DIST_DIR := $(CLDC_OUTPUT_DIR)/$(CLDC_TEMPLATE)/dist
JDK_DIR := $(shell readlink -f `which javac` | xargs dirname)/..

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(BUILD_DIR) && ( \
		ln -s ../src/phoneme/cldc src \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR) && ( \
		$(MAKE) -C src/build/$(CLDC_TEMPLATE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			ENABLE_COMPILATION_WARNINGS=true \
			ENABLE_MAP_FILE=false \
			VERBOSE_BUILD=true \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JVMWorkSpace=$(CLDC_DIR) JVMBuildSpace=$(CLDC_OUTPUT_DIR) \
			JDK_DIR=$(JDK_DIR) \
			EMBOX_GXX=$(EMBOX_GXX) CROSS_COMPILE=$(EMBOX_CROSS_COMPILE) \
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
		$(EMBOX_GXX) -DROMIZING $(ROMIMAGE_EXTRA_CPPFLAGS) -I$(CLDC_DIST_DIR)/include \
			-fpermissive -Wno-error -c ROMImage.cpp && \
		$(EMBOX_GXX) -DROMIZING -I$(CLDC_DIST_DIR)/include \
			-c NativesTable.cpp \
	)
	touch $@

$(INSTALL) :
	ln -s $(CLDC_DIST_DIR)/include $(PKG_INSTALL_DIR)/include
	ln -s $(CLDC_DIST_DIR)/lib/libcldc_vm_g.a $(PKG_INSTALL_DIR)
	ln -s $(CLDC_DIST_DIR)/../target/debug/jvmspi.o $(PKG_INSTALL_DIR)
	# For build hello.jar
	ln -s $(CLDC_DIST_DIR)/bin/preverify $(PKG_INSTALL_DIR)
	ln -s $(CLDC_DIST_DIR)/lib/cldc_classes.zip $(PKG_INSTALL_DIR)
	touch $@
