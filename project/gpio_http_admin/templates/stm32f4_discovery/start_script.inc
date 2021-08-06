
"mount -t DumbFS /dev/stm32flash0 /conf",
"conf_setup",
"netmanager",
"led_driver init",
"flash_settings restore",
"msleep 3000",
"ntpdate 0.europe.pool.ntp.org",
"service httpd http_admin",
