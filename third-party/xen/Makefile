
PKG_NAME := xen
PKG_VER  := 4.13.0

PKG_SOURCES := https://downloads.xenproject.org/release/xen/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5 := d3b13c4c785601be2f104eaddd7c6a00

include $(EXTBLD_LIB)

$(INSTALL) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/ && ( \
		test -d $(INCLUDE_INSTALL_DIR)/xen && rm -rf $(INCLUDE_INSTALL_DIR)/xen; \
		cp -r xen/include/public $(INCLUDE_INSTALL_DIR)/xen \
	)
	touch $@
