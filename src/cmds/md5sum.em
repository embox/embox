$_MODS += md5sum

$_SRCS-md5sum += md5sum.c
$_BRIEF-md5sum = Compute and check MD5 message digest

define $_DETAILS-md5sum
	NAME
		md5sum - compute and check MD5 message digest
	SYNOPSIS
		md5sum [FILE]
	DESCRIPTION
		Print MD5 (128-bit) checksums.
	AUTHORS
		Nikolay Korotky
endef

