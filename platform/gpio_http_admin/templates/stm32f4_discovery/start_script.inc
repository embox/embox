"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",

"ifconfig eth0 192.168.1.128 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 192.168.1.0 netmask 255.255.255.0 eth0",

"ls /dev",

"mount -t DumbFS /dev/stm32flash0 /conf",
"led_driver init",
"flash_settings restore",
"ifconfig",
"service httpd http_admin",
