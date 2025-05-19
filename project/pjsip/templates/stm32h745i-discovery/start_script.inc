"mkdir -s /conf",
"mount -t DumbFS /dev/stm32flash0 /conf",
"conf_setup",
"netmanager",
"ntpdate 0.europe.pool.ntp.org",
"service httpd http_admin",
//"simple_pjsua_imported"
