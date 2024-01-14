To add the library to the project, insert into the mods.conf file statement

	include third_party.lib.libiec60870(iec61850=<true|false>, tls_enable=<true|false>, config="<config name>")

The iec61850=true option indicates that files shared with libiec61850 should be skipped when building lib60870.This option is only necessary if both libraries are included in mods.conf at the same time. By default, iec61850=false.

The tls_enable option defines tls support in the lib60870 stack. By default, tsl_enable=false. If tls_enable=true, then in mods.conf
file you must additionally insert (before include third_party.lib.libiec60870(tls_enable=true..))

	include third_party.lib.mbedtls(tls_config="iec61850")

The config option specifies the name of the stack configuration file (without extension). By default, config="default".
These files are stored in the config folder. Users can be edited or added new file with custom stack options. In the current version
the following configs are available (in addition to default):

* debug - allows output to the console debugging information

See the project/server60870 for examples

API of the lib60870 you can find at https://support.mz-automation.de/doc/lib60870/latest/
