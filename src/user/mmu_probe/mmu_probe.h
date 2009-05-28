#ifndef MMU_PROBE_H_
#define MMU_PROBE_H_

/**
 * handler of command "mmu_probe"
 * It starts tests of mmu mode
 * return 0 if successed
 * return -1 another way
 */
BOOL mmu_probe_shell_handler(int argsc, char **argsv);

#endif /*MMU_PROBE_H_*/
