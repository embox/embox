/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",

"export PWD=/",
"export HOME=/",

/* Setup eth0 interface */
#if 0
"ifconfig eth0 hw ether AA:BB:CC:DD:EE:02 up",
"bootpc eth0",
#else
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 10.0.2.0 netmask 255.255.255.0 eth0",
"route add default gw 10.0.2.10 eth0",
#endif

"ping 10.0.2.10",
"ping 8.8.8.8",
"ping google.com",
"httpd &",
"telnetd &",
"dropbear &",
