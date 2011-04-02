"ifconfig -a 127.0.0.1 -p 255.0.0.0 -m 0:0:0:0:0:0 -u lo",
"route -n 127.0.0.0 -m 255.0.0.0 -d lo add",
"test -t loopback"
