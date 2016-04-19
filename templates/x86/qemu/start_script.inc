/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",

/* Setup eth0 interface */
"netmanager eth0",

"export PWD=/",
"export HOME=/",
