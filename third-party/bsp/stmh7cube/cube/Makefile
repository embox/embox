
define option_get_boolean
$(shell echo OPTION_BOOLEAN_third_party__bsp__stmh7cube__cube__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/bsp/stmh7cube/cube.h -)
endef

PKG_NAME := stm32cubeh7
PKG_VER := v1.8.0

PKG_SOURCES := https://www.github.com/STMicroelectronics/STM32CubeH7/archive/$(PKG_VER).zip

PKG_MD5     := 285f779b056866ed1bd746e7cfec77c7

PKG_PATCHES := component_mt25tl01g.patch

ifeq (0,$(call option_get_boolean,use_middleware_pdm))
PKG_PATCHES += stm32h45i_discovery_audio.patch
endif


include $(EXTBLD_LIB)
