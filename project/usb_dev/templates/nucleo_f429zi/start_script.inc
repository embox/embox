"mkdir -s /conf",
"mount -t DumbFS /dev/stm32flash0 /conf",
"conf_setup",
"netmanager",
"service httpd /http_admin",
"service telnetd",
/* for USB HOST flash drive */
"partition",
"mkdir -s /mnt",
//"mount -t vfat /dev/sda1 /mnt"
