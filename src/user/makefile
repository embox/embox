include $(SCRIPTS_DIR)/autoconf

OBJS-y:=shell_command.o

SUBDIRS-$(MONITOR_USER_ARP)       += arp
SUBDIRS-$(MONITOR_USER_CP)        += cp
SUBDIRS-$(MONITOR_USER_GOTO)      += goto
SUBDIRS-$(MONITOR_USER_HELP)      += help
SUBDIRS-$(MONITOR_USER_LDIMG)     += ldimg
SUBDIRS-$(MONITOR_USER_PNPLS)     += lspnp
SUBDIRS-$(MONITOR_USER_MAN)       += man
SUBDIRS-$(MONITOR_USER_MEM)       += mem
SUBDIRS-$(MONITOR_USER_MMU_PROBE) += mmu_probe
SUBDIRS-$(MONITOR_USER_PING)      += ping
SUBDIRS-$(MONITOR_USER_LS)        += ls
SUBDIRS-$(MONITOR_USER_TESTMEM)   += testmem
SUBDIRS-$(MONITOR_USER_UDPD)      += udpd
SUBDIRS-$(MONITOR_USER_WMEM)      += wmem
SUBDIRS-$(MONITOR_USER_TFTP)      += tftp

include $(SRC_DIR)/common.mk
