"netmanager",
"service telnetd",
"service httpd",
"partition",
"mkdir -v /mnt",
//"mount -t vfat /dev/sd_card1 /mnt",
//"mount -t vfat /dev/sda1 /mnt",
/*
"mkdir -v /conf",
"mount -t DumbFS /dev/stm32flash0 /conf",
"echo 1111 > /conf/1.txt",
"cat /conf/1.txt",
*/
"fb_direct_access",
"tinypy fib.py"
/*
"flash_erase /dev/flash_fix1",
"tftp -g -o /dev/flash_fix1 embox.bin 192.168.2.1",
*/