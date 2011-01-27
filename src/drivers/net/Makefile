
$_PACKAGE := embox.driver.net

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

$_MODS += xemaclite loopback ne2k_pci

$_SRCS-xemaclite += xilinx_emaclite.c
$_SRCS-loopback += loopback.c
$_SRCS-ne2k_pci += ne2k_pci.c
$_DEPS-ne2k_pci += embox.driver.pci

