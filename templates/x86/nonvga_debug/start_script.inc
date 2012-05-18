"ifconfig -a 127.0.0.1 -p 255.0.0.0 -u lo",
"ifconfig -a 10.0.2.16 -p 255.255.255.0 -m AA:BB:CC:DD:EE:02 -u eth0",
"route -n 127.0.0.0 -m 255.0.0.0 -d lo add",
"route -n 10.0.2.0 -m 255.255.255.0 -d eth0 add",
