
$_PACKAGE := embox.usr

$_MODS := $(filter-out mmu_probe mkfile tftp,$($_SUBDIRS))

#$_MODS_ESSENTIAL += $($_MODS)

$(foreach mod,$($_MODS),$(eval $_SRCS-$(mod) += $(mod)/*.c $(mod)/*/*.c))

$_SUBDIRS :=

$_CPPFLAGS += -I$(THIRDPARTY_DIR)/linux-compat/include
