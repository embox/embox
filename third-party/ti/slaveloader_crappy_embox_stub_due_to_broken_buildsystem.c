#include <embox/cmd.h>
#include <assert.h>

EMBOX_CMD(exec);

extern int slaveloader_main(int argc, char **argv);

static int exec(int argc, char **argv) {
        return slaveloader_main(argc, argv);
}

