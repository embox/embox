"mkdir -s /conf",
"mount -t DumbFS /dev/stm32flash0 /conf",
"conf_setup",
"netmanager",
//"flash_settings restore",
"ntpdate 0.europe.pool.ntp.org",
"service httpd http_admin",
"sip_nuklear /dev/stm32-ts"
