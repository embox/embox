
ifdef CONFIG_AMBAPP
$_MODS += ambapp amba_registry
$_SRCS-ambapp += ambapp.c
$_SRCS-amba_registry += amba_registry.c
$_DEPS-amba_registry += ambapp
$_CPPFLAGS-amba_registry +=-I$(SRC_DIR)/drivers/ambapp -I$(SRC_DIR)/linux-compat/include
endif

