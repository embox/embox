"msleep 100",
/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
"ifconfig eth0 192.168.128.128 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:03 up",
"route add 192.168.128.0  netmask 255.255.255.0 eth0",
"route add default gw 192.168.128.1 eth0",
