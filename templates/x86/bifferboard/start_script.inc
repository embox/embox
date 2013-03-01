/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
//"ifconfig -a 10.0.2.16 -p 255.255.255.0 -m AA:BB:CC:DD:EE:02 -u eth0",
//"route -n 10.0.2.0 -g 0.0.0.0 -m 255.255.255.0 -d eth0 add",
//"route -n 0.0.0.0 -g 10.0.2.10 -m 0.0.0.0 -d eth0 add",
