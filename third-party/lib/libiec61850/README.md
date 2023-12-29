To add the library to the project, insert into the mods.conf file statement

	include third_party.lib.libiec61850(tls_enable=<true|false>, config="<config name>")

The tls_enable option defines tls support in the iec61850 stack. By default, tsl_enable=false. If tls_enable=true, then in mods.conf
file you must additionally insert (before third_party.lib.libiec61850(tls_enable=true..))

	include third_party.lib.mbedtls(tls_config="iec61850")

The config option specifies the name of the stack configuration file (without extension). By default, config="default".
These files are stored in the config folder. They can be edited or added new file with custom stack options. In the current version
the following configs are available (in addition to default):

* client_debug - allows output to the console debugging information of the iso_client and mms_client
* server_debug - allows output to the console debugging information of the  socket, copt, iso_server and mms_server
* goose_debug  - allows output to the console debugging information of the  goose_subscriber and goose_publisher
* sv_debug     - allows output to the console debugging information of the  sv_subscriber and sv_publisher

See the project/server61850 for examples

API of the libiec61850 you can find at https://support.mz-automation.de/doc/libiec61850/c/latest/
