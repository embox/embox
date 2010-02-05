/*"ifconfig -a 127.0.0.1 -p 255.0.0.0 -h -m 0:0:0:0:0:0 -u lo",*/
"ifconfig -a 192.168.0.80 -p 255.255.255.0 -m AA:BB:CC:DD:EE:1 -u eth0",
"ifconfig -a 192.168.100.81 -p 255.255.255.0 -m AA:BB:CC:DD:EE:2 -u eth1",
"route -n 192.168.0.0 -m 255.255.255.0 -d eth0 add",
"route -n 192.168.100.0 -m 255.255.255.0 -d eth1 add",
