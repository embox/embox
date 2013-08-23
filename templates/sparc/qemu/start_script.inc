"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 up",
"route add 10.0.2.0 netmask 255.255.255.0 eth0",
"route add default gw 10.0.2.10 eth0"
