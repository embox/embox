
$_MODS += readelf
$_SRCS-readelf += readelf.c
$_DEPS-readelf += embox.fs.core
$_BRIEF-readelf = Display information about the contents of ELF format files

define $_DETAILS-readelf
	NAME
		readelf - shows information about ELF binaries
	SYNOPSIS
		readelf [-hSlrs] <filename>
	DESCRIPTION
		This program shows various information from ELF file
	OPTIONS
		-h 	 Display the ELF file header
		-S 	 Display the sections' header
		-l 	 Display the program headers
		-r 	 Display the relocations
		-s 	 Display the symbol table
	AUTHORS
		Anatoly Trosinenko, Aleksandr Kirov, Nikolay Korotky
endef
