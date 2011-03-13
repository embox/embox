ifdef CONFIG_AMBAPP
$_MODS += lspnp
$_SRCS-lspnp += lspnp.c
$_CPPFLAGS-lspnp += -I$(BASE_CONF_DIR)
$_DEPS-lspnp += embox.driver.amba_registry
endif

$_BRIEF-lspnp = Show list of plug and play devices

define $_DETAILS-lspnp
	NAME
		lspnp - show list of plug and play devices
	SYNOPSIS
		lspnp [-b <bus_type>] [-n <dev_id>]
	OPTIONS
		-b bus_type  - type of the bus of P&P device.
			Possible values: ahbm  - AHB bus, master
				ahbsl - AHB bus, slave
				apb   - APB bus
				all   - AHB&APB buses
			by default: all
		-n dev_id    - number of required device.
	AUTHORS
		Alexandr Batyukov
endef
