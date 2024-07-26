/* Setup loopback interface */
"ifconfig lo 127.0.0.1 netmask 255.0.0.0 up",
"route add 127.0.0.0 netmask 255.0.0.0 lo",
/* Setup eth0 interface */
"ifconfig eth0 10.0.2.16 netmask 255.255.255.0 hw ether AA:BB:CC:DD:EE:02 up",
"route add 10.0.2.0 netmask 255.255.255.0 eth0",
"route add default gw 10.0.2.10 eth0",

"export PWD=/",
"export HOME=/",
"export TCL_LIBRARY=/",

"mkdir -v /tmp",
"mkfs -q /dev/ram0 8192",
"mount -t ramfs  /dev/ram0 /tmp",

"tinypy fib.py",

"chibi-scheme -xscheme fib.scm",

"mruby fib.rb",

"lua lua/fib.lua",
"lua lua/doit.lua",
"lua lua/tofi_mod.lua /tmp/new_file.txt",
"cat /tmp/new_file.txt",

"lua lua/luasocket/http_request.lua http://example.com",

"tclsh run_embox_cmd.tcl",
"tclsh fib.tcl",
"tclsh tcludp_localhost.tcl",

"duk fib.js",

"perl fib.pl",
