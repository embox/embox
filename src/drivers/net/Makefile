
$_PACKAGE := embox.driver.net

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

$_MODS += xemaclite loopback ne2k_pci r6040

$_SRCS-xemaclite += xilinx_emaclite.c
$_SRCS-loopback  += loopback.c
$_SRCS-ne2k_pci  += ne2k_pci.c
$_DEPS-ne2k_pci  += embox.driver.pci
$_SRCS-r6040     += r6040.c
#DEBUG!!
$_DEPS-ne2k_pci  += core.kernel.timer
