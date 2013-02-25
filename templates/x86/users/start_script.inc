/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route -n 127.0.0.0 -g 0.0.0.0 -m 255.0.0.0 -d lo add",
/* Setup eth0 interface */
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route -n 10.0.2.0 -g 0.0.0.0 -m 255.255.255.0 -d eth0 add",
"route -n 0.0.0.0 -g 10.0.2.10 -m 0.0.0.0 -d eth0 add",
"mkdir /tmp/hda",
"mount -t ext2 /dev/hda /tmp/hda",

"smac_adm -R high_label -o low_label  -a r",
"smac_adm -R low_label  -o high_label -a w",
"smac_adm -R _  	-o smac_admin -a r",

"serial_screens 3",
