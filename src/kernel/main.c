/**
 * @file main.c
 *
 * @date 27.11.08
 * @author Anton Bondarev
 */
#include <shell.h>
#include <autoconf.h>

int main() {
#if !defined(SIMULATION_TRG) && defined(MONITOR_SHELL)
        shell_start();
#endif
        while (1)
                ;

        return 0;
}
