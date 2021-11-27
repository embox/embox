"ifconfig eth0 192.168.2.128 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 192.168.2.0 netmask 255.255.255.0 eth0",
"route add default gw 192.168.2.1 eth0",
"record -c 2 -r 16000 -d 1000 -l",
//"play -m 0x20002144" /* see local buffer address during record command */
//"play -s",
//"play -m 0x080a0000", //WAV file should be loaded to ROM first
//"pjsip_simpleua",
