/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route -n 127.0.0.0 -g 0.0.0.0 -m 255.0.0.0 -d lo add",
/* Setup eth0 interface */
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route -n 10.0.2.0 -g 0.0.0.0 -m 255.255.255.0 -d eth0 add",
"route -n 0.0.0.0 -g 10.0.2.10 -m 0.0.0.0 -d eth0 add",
