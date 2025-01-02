
GIT_SHA  := 983cef2542f2052683a041da5c1b49595bc1d75a

PKG_NAME := matiec
PKG_VER  := $(GIT_SHA)

PKG_SOURCES := https://github.com/embox/matiec/archive/$(GIT_SHA).zip
PKG_MD5     := 80c2aba8cb47429d3a144fb54c61d30c

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
