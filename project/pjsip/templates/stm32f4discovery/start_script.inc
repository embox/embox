"ifconfig eth0 192.168.1.128 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 192.168.1.0 netmask 255.255.255.0 eth0",
"route add default gw 192.168.1.1 eth0",
//"play -m 0x080a0000", //WAV file should be loaded to ROM first
"pjsip_simpleua",
