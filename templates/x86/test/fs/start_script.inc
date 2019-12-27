/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 10.0.2.0 netmask 255.255.255.0 eth0",
"route add default gw 10.0.2.10 eth0",

"export PWD=/",
"export HOME=/",

"telnetd &",

"mkdir -v /mnt",
"mkdir -v /mnt/fs_test",

"mkdir -v /tmp",
"mkfs -q /dev/ram0 8192",
"mount -t ramfs  /dev/ram0 /tmp",
