"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",

"ifconfig eth0 192.168.33.100 netmask 255.255.255.0 up",
"route add 192.168.33.0 netmask 255.255.255.0 eth0",
"streamutil",
