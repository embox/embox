/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 10.0.2.0 netmask 255.255.255.0 eth0",
"route add default gw 10.0.2.10 eth0",
/* Env variables */
"export PWD=/",
"export HOME=/",
