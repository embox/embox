//"deepsleep_test",

"msleep 5000",

"ifconfig eth0 10.1.0.1 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 10.1.0.0 netmask 255.255.255.0 eth0",
"service dhcpd",
"service telnetd",
"ble_example_sdk",
