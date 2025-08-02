/* Uncomment for flash device read/write testing and/or using*/
//"block_dev_test -l",
//"block_dev_test stm32flash0",
//"mkdir -s /flash",
//"mount -t DumbFS /dev/stm32flash0 /flash",

"mkdir -s /conf",
//"mount -t DumbFS /dev/stm32flash0 /conf",
//"conf_setup",
"ifconfig eth0 up",
"iwlist eth0 scan",
"iwconfig eth0 essid XXXXXXX key s:XXXXXXX",
