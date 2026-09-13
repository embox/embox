
PKG_NAME    := dronecan_dsdlc
PKG_VER     := 431170fa4bfe2212b516b8f33bdc796267907f1c
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := 65fb34b9a450475aa1a10e2517056ffa

VENV_DIR	:= $(PKG_INSTALL_DIR)/venv
VENV_PYTHON := $(VENV_DIR)/bin/python

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR)

	python3 -m venv $(VENV_DIR)
	$(VENV_PYTHON) -m pip install \
		dronecan \
		empy \
		pexpect \
		ptyprocess

	touch $@
