#include <QtPlugin>

Q_IMPORT_PLUGIN(vnc)
Q_IMPORT_PLUGIN(emboxvc)
Q_IMPORT_PLUGIN(minimal)

extern "C" __attribute__((naked))
void embox_auto_plugin_importer_linker_hook(void) {
	// Just want this compilation module to link in
}
