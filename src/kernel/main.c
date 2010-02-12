/**
 * @file main.c
 *
 * @date 27.11.08
 * @author Anton Bondarev
 */
#include <shell.h>
#include <autoconf.h>

int main(void) {
#if !defined(SIMULATION_TRG) && defined(CONFIG_SHELL)
        shell_start();
#endif
        TRACE("\nend of program\n");
        while (1)
                ;

        return 0;
}
