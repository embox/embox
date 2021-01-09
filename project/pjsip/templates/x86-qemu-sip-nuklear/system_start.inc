"export PWD=/",
"export HOME=/",
"netmanager",

"mkdir -v /tmp",
"mkfs -q /dev/ram0 32768",
"mount -t ramfs /dev/ram0 /tmp",

/* Run simpleua */
//"service pjsip_simpleua",
//"simple_pjsua_imported sip:6002@188.68.209.164",
"tish",
