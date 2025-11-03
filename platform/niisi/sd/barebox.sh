#!/bin/sh

###### Embox ######

memcpy -l -s ./embox.bin 0x0 0xffffffff80001000 0x4000000
go 0xffffffff80001000
