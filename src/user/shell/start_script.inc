//"ifconfig -a 127.0.0.1 -p 255.0.0.0 -h -m 0:0:0:0:0:0 -u lo",
"ifconfig -a 192.168.0.80 -p 255.255.255.0 -m AA:BB:CC:DD:EE:1 -u eth0",
"ifconfig -a 192.168.100.81 -p 255.255.255.0 -m AA:BB:CC:DD:EE:2 -u eth1",
"ifconfig -a 192.168.0.82 -p 255.255.255.0 -m AA:BB:CC:DD:EE:3 -u eth2",
"ifconfig -a 192.168.0.83 -p 255.255.255.0 -m AA:BB:CC:DD:EE:4 -u eth3",
//"route -n 127.0.0.0 -m 255.0.0.0 -d lo add",
"route -n 192.168.0.0 -m 255.255.255.0 -d eth0 add",
"route -n 192.168.100.0 -m 255.255.255.0 -d eth1 add",
"route -g 192.168.0.1 -d eth0 add"
