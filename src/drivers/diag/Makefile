$_PACKAGE = embox.driver.diag

$_MODS 					+= device
$_SRCS-device		  	+= device.c
$_REQUIRES-device		+= embox.hal.diag

$_MODS                  += apbuart
$_SRCS-apbuart  	    += apbuart.c
$_PROVIDES-apbuart 		+= embox.hal.diag

$_MODS                  += serial
$_SRCS-serial	        += serial.c
$_REQUIRES-serial	    += embox.hal.serial
$_PROVIDES-serial  		+= embox.hal.diag

$_MODS                  += vga
$_SRCS-vga	  		    += vga.c
$_REQUIRES-vga	    	+= embox.hal.driver.vga
$_DEPS-vga				+= embox.driver.keyboard
$_PROVIDES-vga  		+= embox.hal.diag
