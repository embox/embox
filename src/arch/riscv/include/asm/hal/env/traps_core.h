/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.07.23
 */
#ifndef RISCV_HAL_ENV_TRAPS_CORE_H_
#define RISCV_HAL_ENV_TRAPS_CORE_H_

typedef int (*__trap_handler)(unsigned long nr, void *data);

typedef struct __traps_env {
} __traps_env_t;

#endif /* RISCV_HAL_ENV_TRAPS_CORE_H_ */
