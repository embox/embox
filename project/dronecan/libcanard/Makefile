
PKG_NAME    := libcanard
PKG_VER     := 9b05a4d8eebf82684053d07e6d04fe2b0c922a6e
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := 9bd8cee80e20e3b0d28a6b6f692b70d3

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
