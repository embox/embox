/* Setup eth0 interface */
"netmanager",

"mkdir -v /tmp",
"mkfs -q /dev/ram0 32768",
"mount -t ramfs /dev/ram0 /tmp",

"cd /tmp",
"pjsip_test",
"tish",
