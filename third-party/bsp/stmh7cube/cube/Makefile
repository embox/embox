
PKG_NAME := stm32cubeh7
PKG_VER  := v1.8.0

PKG_SOURCES := https://github.com/STMicroelectronics/STM32CubeH7/archive/refs/tags/$(PKG_VER).zip
PKG_MD5     := 285f779b056866ed1bd746e7cfec77c7
PKG_PATCHES  = component_mt25tl01g.patch

ifeq (0,$(call option_get,BOOLEAN,use_middleware_pdm))
PKG_PATCHES += stm32h45i_discovery_audio.patch
endif
