include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_DRIVERS_AMBA_PNP)              += amba_pnp

ifeq ($(MONITOR_DRIVERS_AMBA_PNP),y)
SUBDIRS-$(MONITOR_DRIVERS_GAISLER)               += gaisler
endif

include $(SRC_DIR)/common.mk
