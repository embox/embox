PKG_NAME := 4diac-forte
PKG_VER  := 2.0.1
PKG_SOURCES := https://github.com/eclipse-4diac/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := 6be3b6bd8b9276d8c22b8f38f6343515

FORTE_FLAGS = \
		-DFORTE_ARCHITECTURE=Posix \
		-DFORTE_COM_ETH=ON \
		-DFORTE_COM_FBDK=ON \
		-DFORTE_COM_LOCAL=ON \
		-DFORTE_TESTS=OFF \
		-DFORTE_TESTS_INC_DIRS="" \
		-DFORTE_TESTS_LINK_DIRS="" \
		-DFORTE_MODULE_CONVERT=ON \
		-DFORTE_MODULE_IEC61131=ON \
		-DFORTE_MODULE_UTILS=ON \
		-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
		-DCMAKE_CXX_COMPILER:PATH=$(EMBOX_GXX) \
		-DCMAKE_SYSTEM_NAME:STRING=Generic \
		-DCMAKE_BUILD_TYPE:STRING=None \
		-DFORTE_EXTERNAL_MODULES_DIRECTORY=$(PROJECT_DIR)/4diac/forte/Embox_FB_lib \
		-DFORTE_MODULE_EMBOX_LED=ON \ 
		# -DCMAKE_CXX_FLAGS:STRING="$(CXX_FLAGS)" \


$(CONFIGURE):
	export EMBOX_GCC_LINK=full; \
	mkdir -p $(PKG_SOURCE_DIR)/bin/embox; \
	cd $(PKG_SOURCE_DIR)/bin/embox && ( \
		cmake $(FORTE_FLAGS) $(PKG_SOURCE_DIR) \
	)
	touch $@

$(BUILD):
	cd $(PKG_SOURCE_DIR)/bin/embox && ( \
		make \
	)
	touch $@

$(INSTALL):
	mv $(PKG_SOURCE_DIR)/bin/embox/src/forte $(PKG_INSTALL_DIR)/forte.o
	touch $@

