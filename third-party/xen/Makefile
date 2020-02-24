
PKG_NAME := xen
PKG_VER  := stable-4.13

PKG_SOURCES := git://xenbits.xen.org/xen.git

GIT_CLONE_ARGS := -b $(PKG_VER)

PKG_MD5 :=

include $(EXTBLD_LIB)

$(INSTALL) :
	test -d "$(INCUDE_INSTALL_DIR)/xen" && rm -rf "$(INCUDE_INSTALL_DIR)/xen"; \
	cp -r "$(DOWNLOAD_DIR)/xen/xen/include/public" "$(INCUDE_INSTALL_DIR)/xen"
	touch $@
