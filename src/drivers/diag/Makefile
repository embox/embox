$_PACKAGE = embox.driver.diag

$_APIS += diag_api

$_MODS                  += no_diag
$_SRCS-no_diag  	    += no_diag.c
$_PROVIDES-no_diag      += embox.driver.diag.diag_api

$_MODS                  += apbuart
$_SRCS-apbuart  	    += apbuart.c
$_PROVIDES-apbuart 		+= embox.driver.diag.diag_api

$_MODS                  += serial
$_SRCS-serial	        += serial.c
$_REQUIRES-serial	    += embox.driver.serial
$_PROVIDES-serial  		+= embox.driver.diag.diag_api

$_MODS                  += vga
$_SRCS-vga	  		    += vga.c
$_REQUIRES-vga	    	+= embox.hal.driver.vga
$_DEPS-vga				+= embox.driver.keyboard
$_PROVIDES-vga  		+= embox.driver.diag.diag_api
