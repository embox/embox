
GIT_SHA  := 9eba44f372ecc43617c455c8331df93078b338eb

PKG_NAME := matiec
PKG_VER  := $(GIT_SHA)

PKG_SOURCES := https://github.com/embox/matiec/archive/$(GIT_SHA).zip
PKG_MD5     := 42911958253a014c9c83290e0bb99837

include $(EXTBLD_LIB)

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
	$(CP) $(PKG_SOURCE_DIR)/iec2c $(PKG_INSTALL_DIR)
	$(CP) -r $(PKG_SOURCE_DIR)/lib $(PKG_INSTALL_DIR)/lib
	$(MKDIR) $(INCLUDE_INSTALL_DIR)/matiec_lib
	echo > $(PKG_INSTALL_DIR)/lib/C/iec_embox_lib.h
	echo > $(PKG_INSTALL_DIR)/lib/iec_embox_lib.txt
	find $(INCLUDE_INSTALL_DIR)/matiec_lib -name \*.h \
		-exec cat {} >> $(PKG_INSTALL_DIR)/lib/C/iec_embox_lib.h \; \
		-exec printf "\n\n" >> $(PKG_INSTALL_DIR)/lib/C/iec_embox_lib.h \;
	find $(INCLUDE_INSTALL_DIR)/matiec_lib -name \*.iecst \
		-exec cat {} >> $(PKG_INSTALL_DIR)/lib/iec_embox_lib.txt \; \
		-exec printf "\n\n" >> $(PKG_INSTALL_DIR)/lib/iec_embox_lib.txt \;
	printf "\n{disable code generation}\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	printf "\n{#include \"iec_embox_lib.txt\" }\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	printf "\n{enable code generation}\n" >> $(PKG_INSTALL_DIR)/lib/ieclib.txt
	touch $@
