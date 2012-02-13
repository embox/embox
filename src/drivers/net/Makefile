
$_PACKAGE := embox.driver.net

$_CPPFLAGS += -I$(SRC_DIR)/compat/linux/include

$_MODS           += xemaclite
$_SRCS-xemaclite += xilinx_emaclite.c
$_DEPS-xemaclite += embox.kernel.irq

$_MODS           += loopback
$_SRCS-loopback  += loopback.c

$_MODS           += ne2k_pci
$_SRCS-ne2k_pci  += ne2k_pci.c
$_DEPS-ne2k_pci  += embox.driver.pci
$_DEPS-ne2k_pci  += embox.kernel.irq

$_MODS           += r6040
$_SRCS-r6040     += r6040.c
$_DEPS-r6040     += embox.kernel.irq

$_MODS           += rtl8139
$_SRCS-rtl8139   += rtl8139.c
$_DEPS-rtl8139   += embox.driver.pci
$_DEPS-rtl8139   += embox.kernel.irq
