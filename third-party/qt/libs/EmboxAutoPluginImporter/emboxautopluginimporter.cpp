#include <QtPlugin>

#ifdef QEMBOX_PLUGIN_VNC
Q_IMPORT_PLUGIN(vnc)
#endif
#ifdef QEMBOX_PLUGIN_EMBOXVC
Q_IMPORT_PLUGIN(emboxvc)
#endif
Q_IMPORT_PLUGIN(minimal)

extern "C" __attribute__((naked))
void embox_auto_plugin_importer_linker_hook(void) {
	// Just want this compilation module to link in
}
