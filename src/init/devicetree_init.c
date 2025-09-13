#include <kernel/printk.h>

// Forward declaration (from devicetree.c)
int devicetree_init(void);

static int devicetree_bootstrap(void) {
    printk("DT: Running early init...\n");
    return devicetree_init();
}

EMBOX_UNIT_INIT(devicetree_bootstrap);
