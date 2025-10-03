#include <embox/unit.h>
#include <kernel/printk.h>

// Forward declaration (from devicetree.c)
static int devicetree_init(void) {
    printk("DT: Running early init...\n");
    return 0;
}
// Register this function for early initialization
EMBOX_UNIT_INIT(devicetree_init);

