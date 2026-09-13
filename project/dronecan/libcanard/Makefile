
PKG_NAME    := libcanard
PKG_VER     := 601ed35467e0ac38819df17cd7c918de19f62d58
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := 9a61b4cd265ea35d7b04c7e91d4ec092

CANARD_BUILD_DIR= $(PKG_SOURCE_DIR)/build
INSTALL_INC_DIR	= $(PKG_INSTALL_DIR)/include

$(CONFIGURE) :
	$(MKDIR) $(CANARD_BUILD_DIR) $(INSTALL_INC_DIR)
	touch $@

$(BUILD) :
	$(EMBOX_GCC) $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -std=c11 \
		-DCANARD_ENABLE_CANFD=0 -DCANARD_ENABLE_DEADLINE=0 \
		-I$(PKG_SOURCE_DIR) -c $(PKG_SOURCE_DIR)/canard.c \
		-o $(CANARD_BUILD_DIR)/canard.o
	$(dir $(EMBOX_GCC))embox-ar rcs \
		$(CANARD_BUILD_DIR)/libcanard.a $(CANARD_BUILD_DIR)/canard.o
	touch $@

$(INSTALL) :
	$(CP) $(CANARD_BUILD_DIR)/libcanard.a $(PKG_INSTALL_DIR)/
	$(CP) $(PKG_SOURCE_DIR)/canard.h $(INSTALL_INC_DIR)/
	touch $@
