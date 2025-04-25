PKG_NAME := console_print
PKG_VER  := 0.1.0

# CargoFLAGS
TARGET_CFG := i686_32
# Если необходимо то можно переопределить цель из .cargo/config.toml
#CARGO_FLAGS += --target $(TARGET_CFG).json
CARGO_FLAGS += --target-dir $(MOD_BUILD_DIR)
DEBUG ?=

$(info debug is $(DEBUG))

ifdef DEBUG
	RELEASE :=
	TARGET :=debug
else
	RELEASE :=--release
	TARGET :=release
endif

.PHONY : all build install

all: build install

BUILD  := $(MOD_BUILD_DIR)/.built
build : $(BUILD)
$(BUILD):
	export RUSTFLAGS='--print=link-args -Clink-args=$(OBJ_DIR)/module/embox/compat/libc/str.a -L$(OBJ_DIR)/module/embox/compat/libc'; \
	cargo build $(RELEASE) $(CARGO_FLAGS) --verbose; \
	touch $@

PKG_INSTALL_DIR  := $(MOD_BUILD_DIR)/install

INSTALL  := $(MOD_BUILD_DIR)/.installed
install : $(INSTALL)
$(INSTALL):
	cp $(MOD_BUILD_DIR)/$(TARGET_CFG)/$(TARGET)/lib$(PKG_NAME).a $(PKG_INSTALL_DIR)/$(PKG_NAME).a; \
	echo "******* INSTALL DONE!" \
	touch $@
 
help:
	@echo "usage: make $(PKG_NAME) [debug=1]"

clean:
	cargo clean
