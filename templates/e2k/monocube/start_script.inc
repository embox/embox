/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
"ifconfig eth0 192.168.4.155 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 192.168.4.0 netmask 255.255.255.0 eth0",
"route add default gw 192.168.4.1 eth0",
/* Env variables */
"export PWD=/",
"export HOME=/",
"telnetd &",
/* It works: "ticker -c 10" */
