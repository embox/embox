
GIT_SHA  := 307c7f3d5bfc491e9d0e8f1f12ee091908ed339b

PKG_NAME := matiec
PKG_VER  := $(GIT_SHA)

PKG_SOURCES := https://github.com/embox/matiec/archive/$(GIT_SHA).zip
PKG_MD5     := 6c4fc3bd6a1cc091005bf9e044046a3e

include $(EXTBLD_LIB)

IECLIB_DIR := $(PROJECT_DIR)/softplc/ieclib

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && ( \
		autoreconf -i; \
		./configure; \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make; \
	)
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/bin
	$(CP) $(PKG_SOURCE_DIR)/iec2c $(PKG_INSTALL_DIR)/bin
	$(CP) -r $(PKG_SOURCE_DIR)/lib $(PKG_INSTALL_DIR)/lib
	echo > $(PKG_INSTALL_DIR)/lib/embox_ieclib.h
	echo > $(PKG_INSTALL_DIR)/lib/embox_ieclib.iecst
	find $(INCLUDE_INSTALL_DIR)/ieclib -name \*.h \
		-exec cat {} >> $(PKG_INSTALL_DIR)/lib/embox_ieclib.h \; \
		-exec printf "\n\n" >> $(PKG_INSTALL_DIR)/lib/embox_ieclib.h \;
	find $(INCLUDE_INSTALL_DIR)/ieclib -name \*.iecst \
		-exec cat {} >> $(PKG_INSTALL_DIR)/lib/embox_ieclib.iecst \; \
		-exec printf "\n\n" >> $(PKG_INSTALL_DIR)/lib/embox_ieclib.iecst \;
	printf "\n{disable code generation}\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	printf "\n{#include \"embox_ieclib.iecst\" }\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	printf "\n{enable code generation}\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	touch $@
