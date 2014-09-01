/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",

"ifconfig tun0 192.168.0.1 netmask 255.255.255.0 hw ether FE:FE:00:00:00:00 up",
"route add 192.168.0.0 netmask 255.255.255.0 tun0",
"packetdrill packetdrill_tests/simple-connect.pkt",
"packetdrill --tolerance_usec=10000 packetdrill_tests/sudden-close.pkt",
"packetdrill packetdrill_tests/huge-backlog.pkt",
"packetdrill packetdrill_tests/transmission-pause.pkt",

"echo \"run: success auto poweroff\"",
"shutdown -p",
